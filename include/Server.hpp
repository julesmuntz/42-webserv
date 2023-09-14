/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eflaquet <eflaquet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:26 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/09/13 16:59:31 by eflaquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <iterator>
#include <cstring>
#include <exception>
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <list>
#include <deque>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include "RequestHandler.hpp"

#define BACKLOG 10
#define BAD_FD  -1
#define EPOLL_QUEUE_LEN 100000
#define HEADER_MAX_SIZE 10000
#define MSG_MAX_SIZE INT_MAX
#define BUF_SIZE 1024
#define TIMEOUT  1000
#define FILE_CONF ".conf"
#define ERROR_FILENAME "config file does not end with .conf"
#define ERROR_OPEN "error open"
#define ERROR_END "error end"

typedef struct s_location
{
	std::string					uri;
	std::vector<std::string>	allow_methods;
	std::string					root;
	std::vector<std::string>	index; // metre un vecte
	std::string					redir_link; // pas obligatoire file conf si il y est les autre sont pas obligatoire
	std::string					file_location;
	std::string					cgi_pass; // pas obligatoire file conf
	bool						directory_listing; // pas obligatoire file conf
}		t_location;

typedef struct s_server
{
	std::pair<uint32_t, std::string>	listen;
	std::vector<std::string>			server_name;
	uint32_t							client_body_size;
	std::vector<std::pair<uint32_t, std::string> >	error_pages;
	std::vector<t_location>				location;
}		t_server;


class Server
{
	private:
		int							epoll_fd;
		std::vector<int>			sfds;
		struct epoll_event			events[EPOLL_QUEUE_LEN];
		std::map<int, RequestHandler>	requests;
		std::vector<t_server>		con_servs;

		bool		is_listening_socket(int fd);
		int			get_a_socket(int port);
		int			set_up_server(void);
		int			handle_new_connection(int sfd);
		int			receive_data(int i);
		int			send_data(int i);
		static void	sigint_handler(int sig);
		void		memset_events(void);
		void		update_time(void);
		void		shutdown_server(void);
		int			shutdown_server(std::string str_err);

	public:
		Server();
		~Server();
		int		serve_do_your_stuff(void);
		void	set_con_servs(std::vector<t_server> const &co_sers);
};
