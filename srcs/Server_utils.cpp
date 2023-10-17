#include "Server.hpp"

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

t_server	*Server::choose_server(RequestParser rep, t_server *serv)
{
	if (rep.get_req_head().hosts.second == 0)
	{
		std::cout << "ok" << std::endl;
		for (vector<t_server>::iterator it = con_servs.begin(); it != con_servs.end(); it++)
		{
			for (unsigned int i = 0; i < it->server_name.size(); i++)
			{
				std::cout << rep.get_req_head().hosts.first << endl;
				if (it->server_name[i] == rep.get_req_head().hosts.first)
				{
					cout << "je suis la !" <<endl;
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
