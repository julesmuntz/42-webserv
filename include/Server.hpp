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
#include "requestHandling.hpp"

#define PORT "8000"
#define BACKLOG 10
#define BAD_FD  -1
#define EPOLL_QUEUE_LEN 10
#define BUF_SIZE 10
#define FILE_CONF ".conf"
#define ERROR_FILENAME "config file does not end with .conf"
#define ERROR_OPEN "error open"

/*Things a revoir pour le parsing de la config file
		- pouvoir avoir plusieurs inputs (exemple : il peut y avoir plusieurs server_names, separes par whitespaces)
		- accepter les ; a la fin des lignes (maybe)
*/

typedef struct s_location
{
	/*tout ce qui peut etre dans location*/
	std::string				allow_methods;
	std::string				root;
	std::string				index;
	std::string				cgi_pass;
	std::string				client_body_buffer_size;
	bool					alias;
}		t_location;


typedef struct s_server
{
	/*tout ce qui peut etre dans server*/
	std::string				listen;
	std::string				server_name;
	std::string				client_body_buffer_size;
	std::string				error_page;
	std::string				allow_methods;
	std::string				root;
	std::string				link_location;
	std::vector<t_location>	location;
}		t_server;

typedef struct s1_location
{
	/*tout ce qui peut etre dans location*/
	std::string					uri;
	std::vector<std::string>	allow_methods;
	std::string					root;
	std::string					index;
	std::string					redir_link;
	std::string					file_location;
	std::string					cgi_pass;
	bool						directory_listing;
}		t1_location;

typedef struct s1_server
{
	/*tout ce qui peut etre dans server*/
	std::pair<int, std::string>	listen;
	std::vector<std::string>	server_name;
	int							client_body_size;
	std::pair<int, std::string>	error_page;
	std::vector<t1_location>	locations;
}		t1_server;

/* Ce que je voudrais conceptuellement parlant : si il y a une erreur dans la config file rien qu'une,
	elle devient invalide et nous ne prendrons en compte que la config file par defaut,
	attention certains champs peuvent etre vides ou ne pas exister et
	la config file peut rester valide (voir le sujet de webserv) */

/* Ce que je voudrais pour la structure server */
/*  LISTEN = port + adresse | adresse + port = integer pour le port, string pour l'adresse
	SERVER_NAME = vector de string (il peut y en avoir plusieurs, separes par des white spaces)
	CLIENT_BODY_SIZE = integer // ca devrait pas etre buffer mais juste body size
	ERROR_PAGE = numero d'error page + chemin = integer + string
	LOCATION STRUCTS faite de strings principalement
	{
		URI = ce qu'il y a apres le mot cle location
		allow_methods = POST | GET | DELETE = vector de string
		root = un chemin a prendre dans un cas precis, devrait etre associe a un bloc location
		link_location = je ne comprends pas ce champ, tu voulais dire redir link peut-etre ?
		index = chemin par defaut a prendre si c'est le directory
		(NEW / a rajouter) file_location = chemin ou stocker les fichier envoyes par post via cette URI
		directory_listing = accepter que les fichiers soient affiches comme page ou pas = bool (yes or no)
		cgi_pass = le chemin pour executer les cgi (devrait y etre uniquement pour une certaine extension
					sinon, c'est une erreur) => on devrait voir comment le mettre en place ensemble
	}
*/


class Server
{
	private:
		int							epoll_fd;
		std::vector<int>			sfds;
		struct epoll_event			events[10];
		//struct addrinfo				*addr_info;
		std::map<int, t_request>	requests;
		std::vector<t1_server>		con_servs;

		bool		is_listening_socket(int fd);
		int			get_a_socket(int port);
		int			set_up_server(void);
		int			handle_new_connection(int sfd);
		int			receive_data(int i);
		int			send_data(int i);
		static void	sigint_handler(int sig);
		void		memset_events(void);
		void		shutdown_server();

	public:
		Server();
		~Server();
		int	serve_do_your_stuff(void);
		void	set_con_servs(std::vector<t1_server> const &co_sers);
};
