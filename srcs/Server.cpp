/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelrhaz <mbelrhaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:05 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/10/16 21:14:48 by mbelrhaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ResponseHTTP.hpp"

bool g_stop_required = 0;

Server::Server() : epoll_fd(-1)
{
	this->memset_events();
}

Server::~Server() {}

/* Handles CTR+C */

void Server::sigint_handler(int sig)
{
	(void)sig;
	g_stop_required = 1;
}

/* Gets the address of the server, creates a socket and binds it to
   the chosen port */

int Server::get_a_socket(int port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	stringstream port_sstream;
	int sfd, ret;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	port_sstream << port;
	ret = getaddrinfo(NULL, port_sstream.str().c_str(), &hints, &result);
	if (ret)
	{
		cerr << "getaddrinfo: " << gai_strerror(ret) << endl;
		return (1);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;
		int optval = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval,
					   sizeof(optval)) == -1)
		{
			close(sfd);
			freeaddrinfo(result);
			return (this->shutdown_server("setsockopt"));
		}
		cout << "sfd = " << sfd << endl;
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}

	freeaddrinfo(result);

	if (rp == NULL)
	{
		cerr << "Could not bind" << endl;
		return (1);
	}
	sfds.push_back(sfd);
	return (sfd);
}

/* Sets up the server, by creating the listening sockets associated
   with the chosen ports, by creating an epoll and making the
   listening sockets listen() to entrant connections */

int Server::set_up_server(void)
{
	struct epoll_event event;
	int sfd;

	signal(SIGINT, Server::sigint_handler);
	signal(SIGQUIT, Server::sigint_handler);
	epoll_fd = epoll_create(EPOLL_QUEUE_LEN);
	if (epoll_fd == -1)
	{
		return (this->shutdown_server("epoll_create"));
	}
	for (vector<t_server>::iterator it = con_servs.begin();
		 it != con_servs.end(); it++)
	{
		sfd = this->get_a_socket(it->listen.first);
		if (sfd == 1)
		{
			this->shutdown_server();
			return (1);
		}
		memset(&event, 0, sizeof(event));
		event.events = EPOLLIN;
		event.data.fd = sfd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sfd, &event))
		{
			return (this->shutdown_server("epoll_ctl"));
		}
		if (listen(sfd, 10) == -1)
		{
			return (this->shutdown_server("listen"));
		}
	}
	return (0);
}

/* Handles new entrant connections to the listening sockets.
   We use accept() to accept the new connections then add them to epoll */

int Server::handle_new_connection(int sfd)
{
	struct epoll_event event;
	struct sockaddr sock_addr;
	socklen_t sock_len = 0;
	int connfd;

	connfd = accept(sfd, &sock_addr, &sock_len);
	if (connfd == -1)
	{
		perror("accept");
		return (0);
	}
	memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = connfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &event) == -1)
	{
		perror("epoll_ctl: conn_sock");
		return (0);
	}
	cout << "NEW fd = " << connfd << " added to epoll" << endl;
	RequestHandler req(connfd);
	requests.insert(pair<int, RequestHandler>(connfd, req));
	return (0);
}

/* Handles the data sent to connection sockets by the client */

int Server::receive_data(int i)
{
	struct epoll_event event;
	ssize_t nread;
	char buf[BUF_SIZE + 1];

	nread = recv(events[i].data.fd, buf, BUF_SIZE, 0);
	if (nread == -1)
	{
		perror("recv");
		//return (this->shutdown_server("recv"));
	}
	if (nread == 0)
	{
		cerr << "CONNEXION CLOSED BY CLIENT" << endl
			 << endl;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1)
			perror("epoll_ctl");
			//return (this->shutdown_server("epoll_ctl"));
		close(events[i].data.fd);
		requests.erase(events[i].data.fd);
		return (0);
	}
	buf[nread] = '\0';
	if (!(requests.find(events[i].data.fd))->second.add_data(buf, nread))
	{
		requests.find(events[i].data.fd)->second.deactivate_timeout();
		requests.find(events[i].data.fd)->second.check_preparsing_errors();
		RequestParser parsedRequest = RequestParser(requests.find(events[i].data.fd)->second.get_request_string());
		t_server serv;
		choose_server(parsedRequest, &serv);
		string uri = parsedRequest.get_uri();
		for (size_t j = 0; j < serv.location.size(); ++j)
		{
			const t_location &loc = serv.location[j];
			if (uri.find(loc.uri) == 0 && !loc.cgi_script.empty())
			{
				serv.cgi = true;
				break;
			}
		}

		ResponseHTTP responseHTTP(parsedRequest, &serv, requests.find(events[i].data.fd)->second.get_error());
		ResponseSender resp(events[i].data.fd, responseHTTP.get_response_string(), parsedRequest.get_chunked());
		responses.insert(pair<int, ResponseSender>(events[i].data.fd, resp));
		memset(&event, 0, sizeof(event));
		event.events = EPOLLOUT;
		event.data.fd = events[i].data.fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1)
			perror("epoll_ctl");
			//return (this->shutdown_server("epoll_ctl"));
	}
	return (0);
}

/* Sends data through a connection socket to the client,
   then closes the connection socket (close the connection) */

int Server::send_data(int i)
{
	// check return value of send
	if (responses.find(events[i].data.fd)->second.send_response())
	{
		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1)
			return (this->shutdown_server("epoll_ctl"));
		close(events[i].data.fd);
		requests.erase(events[i].data.fd);
		responses.erase(events[i].data.fd);
	}
	return (0);
}

/* Server Work, open sockets, receive requests, send responses */

int Server::serve_do_your_stuff(void)
{
	int event_count;

	if (this->set_up_server())
	{
		return (1);
	}
	while (!g_stop_required)
	{
		this->update_time();
		event_count = epoll_wait(epoll_fd, events, EPOLL_QUEUE_LEN, 1000);
		if (event_count == -1 && errno == EINTR)
			break;
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