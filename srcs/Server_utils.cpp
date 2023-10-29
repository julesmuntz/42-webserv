#include "Server.hpp"

/* Initializes the events queue */

void	Server::memset_events(void)
{
	for (int i = 0; i < EPOLL_QUEUE_LEN; i++)
	{
		events[i].data.fd = -1;
	}
}

/* Sets context servers from the config file */

void	Server::set_con_servs(vector<t_server> const &co_sers)
{
	this->con_servs = co_sers;
}

t_server	*Server::choose_server(RequestParser rep, t_server *serv)
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
					return (serv);
				}
			}
		}
	}
	for (vector<t_server>::iterator it = con_servs.begin(); it != con_servs.end(); it++)
	{
		if (it->listen.first == rep.get_req_head().hosts.second && it->listen.second == rep.get_req_head().hosts.first)
		{
			*serv = *it;
			return (serv);
		}
	}
	return (NULL);
}

/* Shuts down the server, closes and frees everything,
   writes down error message if necessary with the overload */

void Server::shutdown_server(void)
{
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
	for (map<int, int>::iterator it = connfds.begin(); it != connfds.end(); it++)
	{
		if (it->second != -1)
			close(it->second);
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
	for (map<int, int>::iterator it = connfds.begin(); it != connfds.end(); it++)
	{
		if (it->second != -1)
			close(it->second);
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

void Server::update_time(void)
{
	map<int, RequestHandler>::iterator	it;

	for (it = requests.begin(); it != requests.end(); it++)
	{
		int	fd;
		struct epoll_event event;

		if (it->second.check_timeout())
		{
			fd = it->second.get_fd();
			it->second.deactivate_timeout();
			it->second.check_preparsing_errors();
			RequestParser parsedRequest = it->second.get_request_string();
			parsedRequests.insert(pair<int, RequestParser>(fd, parsedRequest));
			t_server serv;
			t_server *p_serv = choose_server(parsedRequest, &serv);
			ResponseHTTP responseHTTP =  ResponseHTTP(parsedRequest, p_serv, it->second.get_error(), this);
			responseHTTPs.insert(pair<int, ResponseHTTP>(fd, responseHTTP));
			memset(&event, 0, sizeof(event));
			event.events = EPOLLOUT;
			event.data.fd = fd;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event) == -1)
				perror("epoll_ctl: mod");
		}
	}

	map<int, ResponseHTTP>::iterator	ite;

	for (ite = responseHTTPs.begin(); ite != responseHTTPs.end(); ite++)
	{
		if (ite->second.get_need_cgi() && ite->second.check_timeout())
		{
			ite->second.deactivate_timeout();
			ite->second.set_need_cgi(false);
			ite->second._delete_env();
			int write_fd = ite->second.get_write();
			if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, write_fd, NULL) == -1)
					perror("epoll_ctl write_fd in update_time");
			writePipe.erase(write_fd);
			if (write_fd != -1)
				close(write_fd);
			ite->second.close_pipes();
			ite->second.kill_child();
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
