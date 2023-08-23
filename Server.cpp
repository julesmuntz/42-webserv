/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelrhaz <mbelrhaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:05 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/08/20 18:11:11 by mbelrhaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	g_stop_required = 0;

Server::Server() {}

Server::~Server() {}

void	Server::sigint_handler(int sig)
{
	(void)sig;
	g_stop_required = 1;
}

void	Server::shutdown_server(void)
{
	std::cout << std::endl << "Shutting down" << std::endl;
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		if (events[i].data.fd != -1)
			close(events[i].data.fd);
	}
	if (sfd != -1)
		close(sfd);
	close(epoll_fd);
}

void	Server::memset_events(void)
{
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		events[i].data.fd = -1;
	}
}

//gets the address of the server, creates a socket and binds it
//to bind means to bind a unique local name to the socket with its file descriptor
//bind a name to a socket
//after calling socket(), a descriptor does not have a name associated with it
//try to understand bind() and sockets further by watching videos maybe, it is a bit obscure
int	Server::get_a_socket(void)
{
	struct addrinfo	hints;
	struct addrinfo	*result, *rp;
	int				sfd, ret;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	ret = getaddrinfo(NULL, PORT, &hints, &result);
	if (ret)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(ret) << std::endl;
		return (BAD_FD);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;
		int optval = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
			close(sfd);
		fcntl(sfd, F_SETFL, O_NONBLOCK);
		std::cout << "sfd = " << sfd << std::endl;
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}

	freeaddrinfo(result);

	if (rp == NULL)
	{
		std::cerr << "Could not bind" << std::endl;
		return (BAD_FD);
	}
	addr_info = rp;
	return (sfd);
}

int	Server::set_up_server(void)
{
	struct epoll_event event;

	signal(SIGINT, Server::sigint_handler);
	epoll_fd = epoll_create(EPOLL_QUEUE_LEN);
	if (epoll_fd == -1)
	{
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		return (1);
	}
	sfd = this->get_a_socket();
	if (sfd == BAD_FD)
	{
		close(epoll_fd);
		return (2);
	}
	event.events = EPOLLIN;
	event.data.fd = sfd; // this is the listening socket,
	//later we add the connection sockets for exchanging data, that are returned by accept()
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sfd, &event))
	{
		std::cout << "Failed to add file descriptor to epoll" << std::endl;
		close(sfd);
		close(epoll_fd);
		return (3);
	}
	if (listen(sfd, 10) == -1)
	{
		perror("listen");
		close(sfd);
		close(epoll_fd);
		return (4);
	}
	this->memset_events();
	return (0);
}

int	Server::handle_new_connection(void)
{
	struct epoll_event	event;
	struct sockaddr		sock_addr;
	socklen_t			sock_len = 0;
	int					connfd;

	connfd = accept(sfd, &sock_addr, &sock_len);
	if (connfd == -1)
	{
		perror("accept");
		this->shutdown_server();
		return (6);
	}
	fcntl(connfd, F_SETFL, O_NONBLOCK);
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = connfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &event) == -1)
	{
		perror("epoll_ctl: conn_sock");
		this->shutdown_server();
		return (7);
	}
	std::cout << "NEW fd = " << connfd << " added to epoll" << std::endl;
	return (0);
}

int	Server::receive_data(int i)
{
	struct epoll_event	event;
	ssize_t				nread;
	char				buf[BUF_SIZE + 1];

	nread = recv(events[i].data.fd, buf, BUF_SIZE, 0);
	buf[nread] = 0;
	std::cout << buf;
	while (nread >= 2 && buf[nread] != '\n' && buf[nread - 1] != '\n')
	{
		nread = recv(events[i].data.fd, buf, BUF_SIZE, 0);
		buf[nread] = 0;
		std::cout << buf;
	}
	// reading or receiving is over, now let's get to writing or sending
	event.events = EPOLLOUT | EPOLLET;
	event.data.fd = events[i].data.fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1)
	{
		perror("epoll_ctl: mod did not work");
		this->shutdown_server();
		return (8);
	}
	return (0);
}

int	Server::send_data(int i)
{
	struct epoll_event	event;

	std::cout << "Sending..." << std::endl;
	send(events[i].data.fd, "HTTP/1.1 200 OK\n\n<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body>houhou</body></html>\n\n",
		strlen("HTTP/1.1 200 OK\n\n<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body>houhou</body></html>\n\n"), 0);
	std::cout << "Send OK" << std::endl;
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = events[i].data.fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event) == -1)
	{
		perror("epoll_ctl: del did not work");
		this->shutdown_server();
		return (9);
	}
	close(events[i].data.fd);
	return (0);
}

//each request should be associated with an fd and we should check if we read the end or not each time, if the end, cool, if not, add it to the existing structure
//create a map of fd-request
int	Server::serve_do_your_stuff(void)
{
	int	event_count;

	if (int ret = this->set_up_server())
		return (ret);
	while (!g_stop_required)
	{
		event_count = epoll_wait(epoll_fd, events, EPOLL_QUEUE_LEN, 10000);
		if (event_count == -1 && errno == EINTR)
			break ;
		if (event_count == -1)
		{
			perror("epoll_wait");
			this->shutdown_server();
			return (5);
		}
		for (int i = 0; i < event_count; i++)
		{
			if (events[i].data.fd == sfd)
			{
				if (int ret = this->handle_new_connection())
					return (ret);
			}
			else
			{
				if (events[i].events == EPOLLIN)
				{
					if (int ret = this->receive_data(i))
						return (ret);
				}
				if (events[i].events == EPOLLOUT)
				{
					if (int ret = this->send_data(i))
						return (ret);
				}
			}
		}
	}
	this->shutdown_server();
	return (0);
}
