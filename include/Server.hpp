/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelrhaz <mbelrhaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:26 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/08/20 18:23:56 by mbelrhaz         ###   ########.fr       */
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

#define PORT "8000"
#define BACKLOG 10
#define BAD_FD  -1
#define EPOLL_QUEUE_LEN 10
#define BUF_SIZE 10000

class Server
{
	private:
		int							epoll_fd;
		int							sfd;
		struct epoll_event			events[10];
		struct addrinfo				*addr_info;
		std::map<int, std::string>	requests;

		int			get_a_socket(void);
		int			set_up_server(void);
		int			handle_new_connection(void);
		int			receive_data(int i);
		int			send_data(int i);
		static void	sigint_handler(int sig);
		void		memset_events(void);
		void		shutdown_server();

	public:
		Server();
		~Server();
		int	serve_do_your_stuff(void);
};
