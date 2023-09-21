#pragma once

#include "RequestHandler.hpp"
#include "RequestParser.hpp"
#include "ResponseSender.hpp"
#include <map>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <errno.h>
#include <signal.h>
#include <map>

using namespace std;

#define BACKLOG 10
#define EPOLL_QUEUE_LEN 100000
#define BUF_SIZE 1024
#define TIMEOUT  1000
#define FILE_CONF ".conf"
#define FILE_DEFAULT "conf/default.conf"
#define ERROR_FILENAME "config file does not end with .conf"
#define ERROR_OPEN "error open"
#define ERROR_END "error end"


typedef struct s_location
{
	string			uri;
	vector<string>	allow_methods;
	string			root;
	vector<string>	index;
	string			redir_link;
	string			file_location;
	string			cgi_pass;
	bool			directory_listing;
}		t_location;

typedef struct s_server
{
	pair<uint32_t, string>			listen;
	vector<string>					server_name;
	uint32_t						client_body_size;
	map<uint32_t, string>			error_pages;
	vector<t_location>				location;
}		t_server;


class Server
{
	private:
		int							epoll_fd;
		vector<int>					sfds;
		struct epoll_event			events[EPOLL_QUEUE_LEN];
		map<int, RequestHandler>	requests;
		map<int, ResponseSender>	responses;
		vector<t_server>			con_servs;

		t_server	choose_server(RequestParser rep, bool &no_server);
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
		int			shutdown_server(string str_err);

	public:
		Server();
		~Server();
		int		serve_do_your_stuff(void);
		void	set_con_servs(vector<t_server> const &co_sers);
};
