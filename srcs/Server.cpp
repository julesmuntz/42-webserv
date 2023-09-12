/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eflaquet <eflaquet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:05 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/09/12 18:32:08 by eflaquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	g_stop_required = 0;

Server::Server() : epoll_fd(-1) {
	this->memset_events();
}

Server::~Server() {}

/* Handles CTR+C */

void	Server::sigint_handler(int sig)
{
	(void)sig;
	g_stop_required = 1;
}

/* Shuts down the server, closes and frees everything,
   writes down error message if necessary with the overload */

void	Server::shutdown_server(void)
{
	std::cout << std::endl << "Shutting down" << std::endl;
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		if (events[i].data.fd != -1)
			close(events[i].data.fd);
	}
	for (std::vector<int>::iterator it = sfds.begin(); it != sfds.end(); it++)
	{
		if (*it != -1)
			close(*it);
	}
	if (epoll_fd != -1)
		close(epoll_fd);
}

int	Server::shutdown_server(std::string str_err)
{
	perror(str_err.c_str());
	std::cout << std::endl << "Shutting down" << std::endl;
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		if (events[i].data.fd != -1)
			close(events[i].data.fd);
	}
	for (std::vector<int>::iterator it = sfds.begin(); it != sfds.end(); it++)
	{
		if (*it != -1)
			close(*it);
	}
	if (epoll_fd != -1)
		close(epoll_fd);
	return (1);
}

/* Initializes the events queue */

void	Server::memset_events(void)
{
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		events[i].data.fd = -1;
	}
}

/* Sets context servers from the config file */

void	Server::set_con_servs(std::vector<t_server> const &co_sers)
{
	this->con_servs = co_sers;
}

void	Server::update_time(void)
{
	std::map<int, RequestHandler>::iterator	it;
	struct epoll_event						event;

	for (it = requests.begin(); it != requests.end(); it++)
	{
		if (it->second.check_timeout())
		{
			it->second.deactivate_timeout();
			event.events = EPOLLOUT;
			event.data.fd = it->first;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, it->first, &event) == -1)
				this->shutdown_server("epoll_ctl");
		}
	}
}

/* Returns true if the fd is a listening socket */

bool	Server::is_listening_socket(int fd)
{
	for (std::vector<int>::iterator it = sfds.begin(); it != sfds.end(); it++)
	{
		if (*it == fd)
			return (true);
	}
	return (false);
}

/* Gets the address of the server, creates a socket and binds it to
   the chosen port */

int	Server::get_a_socket(int port)
{
	struct addrinfo		hints;
	struct addrinfo		*result,	*rp;
	std::stringstream	port_sstream;
	int					sfd, ret;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	port_sstream << port;
	ret = getaddrinfo(NULL, port_sstream.str().c_str(), &hints, &result);
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
		int	optval = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval,
				sizeof(optval)) == -1)
		{
			close(sfd);
			return (this->shutdown_server("setsockopt"));
		}
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
	sfds.push_back(sfd);
	return (sfd);
}

/* Sets up the server, by creating the listening sockets associated
   with the chosen ports, by creating an epoll and making the
   listening sockets listen() to entrant connections */

int	Server::set_up_server(void)
{
	struct epoll_event	event;
	int					sfd;

	signal(SIGINT, Server::sigint_handler);
	epoll_fd = epoll_create(EPOLL_QUEUE_LEN);
	if (epoll_fd == -1)
	{
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		return (1);
	}
	for (std::vector<t_server>::iterator it = con_servs.begin();
			it != con_servs.end(); it++)
	{
		sfd = this->get_a_socket(it->listen.first);
		if (sfd == BAD_FD)
		{
			this->shutdown_server();
			return (2);
		}
		event.events = EPOLLIN;
		event.data.fd = sfd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sfd, &event))
		{
			std::cout << "Failed to add file descriptor to epoll" << std::endl;
			this->shutdown_server();
			return (3);
		}
		if (listen(sfd, 10) == -1)
		{
			close(sfd);
			return (this->shutdown_server("listen"));
		}
	}
	return (0);
}

/* Handles new entrant connections to the listening sockets.
   We use accept() to accept the new connections then add them to epoll */

int	Server::handle_new_connection(int sfd)
{
	struct epoll_event	event;
	struct sockaddr		sock_addr;
	socklen_t			sock_len = 0;
	int					connfd;

	connfd = accept(sfd, &sock_addr, &sock_len);
	if (connfd == -1)
		return (this->shutdown_server("accept"));
	event.events = EPOLLIN;
	event.data.fd = connfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &event) == -1)
		return (this->shutdown_server("epoll_ctl: conn_sock"));
	std::cout << "NEW fd = " << connfd << " added to epoll" << std::endl;
	RequestHandler	req(connfd);
	requests.insert(std::pair<int, RequestHandler>(connfd, req));
	return (0);
}

/* Handles the data sent to connection sockets by the client */

int	Server::receive_data(int i)
{
	struct epoll_event	event;
	ssize_t				nread;
	char				buf[BUF_SIZE + 1];

	nread = recv(events[i].data.fd, buf, BUF_SIZE, 0);
	if (nread == -1)
	{
		return (this->shutdown_server("recv"));
	}
	if (nread == 0)
	{
		std::cout << "CLOSE" << nread << std::endl << std::endl;
		event.events = EPOLLIN;
		event.data.fd = events[i].data.fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event) == -1)
			return (this->shutdown_server("epoll_ctl"));
		close(events[i].data.fd);
		requests.erase(events[i].data.fd);
		return (0);
	}
	buf[nread] = '\0';
	if (!(requests.find(events[i].data.fd))->second.add_data(buf))
	{
		requests.find(events[i].data.fd)->second.deactivate_timeout();
		event.events = EPOLLOUT;
		event.data.fd = events[i].data.fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1)
			return (this->shutdown_server("epoll_ctl"));
	}
	return (0);
}

/* Sends data through a connection socket to the client,
   then closes the connection socket (close the connection) */

int	Server::send_data(int i)
{
	struct epoll_event	event;

	std::cout << "Sending..." << std::endl;
	//check return value of send
	send(events[i].data.fd, "HTTP/1.1 200 OK\n\n<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body>houhou</body></html>\n\n",
		strlen("HTTP/1.1 200 OK\n\n<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body>houhou</body></html>\n\n"), 0);
	std::cout << "Send OK" << std::endl;
	event.events = EPOLLIN;
	event.data.fd = events[i].data.fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event) == -1)
		return (this->shutdown_server("epoll_ctl"));
	close(events[i].data.fd);
	requests.erase(events[i].data.fd);
	return (0);
}

/* Server Work, open sockets, receive requests, send responses */

int	Server::serve_do_your_stuff(void)
{
	int	event_count;

	if (int ret = this->set_up_server())
	{
		this->shutdown_server();
		return (ret);
	}
	while (!g_stop_required)
	{
		this->update_time();
		event_count = epoll_wait(epoll_fd, events, EPOLL_QUEUE_LEN, 1000);
		if (event_count == -1 && errno == EINTR)
			break ;
		if (event_count == -1)
			return (this->shutdown_server("epoll_wait"));
		for (int i = 0; i < event_count; i++)
		{
			if (this->is_listening_socket(events[i].data.fd))
			{
				if (this->handle_new_connection(events[i].data.fd))
					return (1);
			}
			else
			{
				if (events[i].events == EPOLLIN)
				{
					if (this->receive_data(i))
						return (1);
				}
				if (events[i].events == EPOLLOUT)
				{
					if (this->send_data(i))
						return (1);
				}
			}
		}
	}
	this->shutdown_server();
	return (0);
}
