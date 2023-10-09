/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eflaquet <eflaquet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:05 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/10/09 15:21:14 by eflaquet         ###   ########.fr       */
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

/* Shuts down the server, closes and frees everything,
   writes down error message if necessary with the overload */

void Server::shutdown_server(void)
{
	cout << endl
		 << "Shutting down" << endl;
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		if (events[i].data.fd != -1)
			close(events[i].data.fd);
	}
	for (vector<int>::iterator it = sfds.begin(); it != sfds.end(); it++)
	{
		if (*it != -1)
			close(*it);
	}
	if (epoll_fd != -1)
		close(epoll_fd);
}

int Server::shutdown_server(string str_err)
{
	perror(str_err.c_str());
	cout << endl
		 << "Shutting down" << endl;
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		if (events[i].data.fd != -1)
			close(events[i].data.fd);
	}
	for (vector<int>::iterator it = sfds.begin(); it != sfds.end(); it++)
	{
		if (*it != -1)
			close(*it);
	}
	if (epoll_fd != -1)
		close(epoll_fd);
	return (1);
}

/* Initializes the events queue */

void Server::memset_events(void)
{
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		events[i].data.fd = -1;
	}
}

/* Sets context servers from the config file */

void Server::set_con_servs(vector<t_server> const &co_sers)
{
	this->con_servs = co_sers;
}

void Server::update_time(void)
{
	map<int, RequestHandler>::iterator it;
	struct epoll_event event;

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

void Server::choose_server(RequestParser rep, t_server *serv)
{
	if (rep.get_req_head().hosts.second == 0)
	{
		for (vector<t_server>::iterator it = con_servs.begin(); it != con_servs.end(); it++)
		{
			for (unsigned int i = 0; i < it->server_name.size(); i++)
			{
				if (it->server_name[i] == rep.get_req_head().hosts.first)
				{
					*serv = *it;
				}
			}
		}
	}
	for (vector<t_server>::iterator it = con_servs.begin(); it != con_servs.end(); it++)
	{
		if (it->listen.first == rep.get_req_head().hosts.second && it->listen.second == rep.get_req_head().hosts.first)
		{
			*serv = *it;
		}
	}
}

/* Returns true if the fd is a listening socket */

bool Server::is_listening_socket(int fd)
{
	for (vector<int>::iterator it = sfds.begin(); it != sfds.end(); it++)
	{
		if (*it == fd)
			return (true);
	}
	return (false);
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
		return (this->shutdown_server("accept"));
	memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = connfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &event) == -1)
		return (this->shutdown_server("epoll_ctl: conn_sock"));
	cout << "NEW fd = " << connfd << " added to epoll" << endl;
	RequestHandler req(connfd);
	requests.insert(pair<int, RequestHandler>(connfd, req));
	return (0);
}

 void set_environment_variables(RequestParser &rp, string cgi_script, string file_location, vector<t_FileInfo>::const_iterator it)
{
	clearenv();
	(void)rp;(void) cgi_script;
	string query_string = "name=" + it->fileName + "&path=cgi-bin/.tmp&dir=" + file_location;
	setenv("QUERY_STRING", query_string.c_str(), 1);
	// cout << "QUERY " << getenv("QUERY_STRING") << endl;
	// setenv("UPLOAD_FILE_NAME", it->fileName.c_str(), 1);
	// setenv("UPLOAD_FILE_TYPE", it->fileType.c_str(), 1);
	// setenv("UPLOAD_FILE_PATH", "cgi-bin/.tmp", 1);
	// setenv("REQUEST_METHOD", rp.get_method().c_str(), 1);
	// setenv("CONTENT_LENGTH", rp.get_rep_head().content_length.c_str(), 1);
	// setenv("CONTENT_TYPE", rp.get_rep_head().content_type.c_str(), 1);
	// string scriptName = "cgi-bin/" + cgi_script;
	// setenv("SCRIPT_NAME", scriptName.c_str(), 1);
	// setenv("REMOTE_ADDR", "127.0.0.1", 1);
	// setenv("REMOTE_PORT", "80", 1);
	// setenv("SERVER_SOFTWARE", "WebServ/1.42", 1);
	// setenv("SERVER_NAME", rp.get_req_head().hosts.first.c_str(), 1);
	// setenv("SERVER_PORT", uint32_to_string(rp.get_req_head().hosts.second).c_str(), 1);
	// setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	// setenv("UPLOAD_DIR", file_location.c_str(), 1);
	// setenv("HTTP_USER_AGENT", rp.get_req_head().user_agent.c_str(), 1);

	// cout << "UPLOAD_FILE_NAME:    " << getenv("UPLOAD_FILE_NAME") << endl;
	// cout << "UPLOAD_FILE_TYPE:    " << getenv("UPLOAD_FILE_TYPE") << endl;
	// cout << "UPLOAD_FILE_CONTENT: " << max_chars(getenv("UPLOAD_FILE_CONTENT"), 350) << endl;
	// cout << "REQUEST_METHOD:      " << getenv("REQUEST_METHOD") << endl;
	// cout << "CONTENT_LENGTH:      " << getenv("CONTENT_LENGTH") << endl;
	// cout << "CONTENT_TYPE:        " << getenv("CONTENT_TYPE") << endl;
	// cout << "SCRIPT_NAME:         " << getenv("SCRIPT_NAME") << endl;
	// cout << "REMOTE_ADDR:         " << getenv("REMOTE_ADDR") << endl;
	// cout << "REMOTE_PORT:         " << getenv("REMOTE_PORT") << endl;
	// cout << "SERVER_SOFTWARE:     " << getenv("SERVER_SOFTWARE") << endl;
	// cout << "SERVER_NAME:         " << getenv("SERVER_NAME") << endl;
	// cout << "SERVER_PORT:         " << getenv("SERVER_PORT") << endl;
	// cout << "SERVER_PROTOCOL:     " << getenv("SERVER_PROTOCOL") << endl;
	// cout << "UPLOAD_DIR:          " << getenv("UPLOAD_DIR") << endl;
	// cout << "HTTP_USER_AGENT:     " << getenv("HTTP_USER_AGENT") << endl;
}

void handle_cgi_request(string cgi_script)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0)
	{
		close(pipefd[1]);
		if (dup2(pipefd[0], STDIN_FILENO) == -1)
		{
			perror("dup2");
			exit(EXIT_FAILURE);
		}
		string cmd = "./cgi-bin/" + cgi_script + " > output.txt 2>&1";
		std::system(cmd.c_str());
		exit(EXIT_FAILURE);
	}
	else
	{
		close(pipefd[0]);
		close(pipefd[1]);
		wait(NULL);
	}
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
		return (this->shutdown_server("recv"));
	}
	if (nread == 0)
	{
		cerr << "CONNEXION CLOSED BY CLIENT" << endl
			 << endl;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1)
			return (this->shutdown_server("epoll_ctl"));
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
				parsedRequest.parseFile();
				for (vector<t_FileInfo>::const_iterator it = parsedRequest.get_fileInfo().begin(); it != parsedRequest.get_fileInfo().end(); it++)
				{
					std::ofstream outfile("cgi-bin/.tmp");
					if (!outfile.is_open())
						break;
					outfile << it->fileContent;
					outfile.close();
					set_environment_variables(parsedRequest, loc.cgi_script, loc.file_location, it);
					handle_cgi_request(loc.cgi_script);
					remove("cgi-bin/.tmp");
				}
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
			return (this->shutdown_server("epoll_ctl"));
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
