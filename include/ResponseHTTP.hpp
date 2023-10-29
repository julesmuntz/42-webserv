#pragma once

#include "ResponseError.hpp"
#include "Server.hpp"

#define DUMMY_RESPONSE	"HTTP/1.1 200 OK\n\n<!DOCTYPE html><html lang=\"en\"><head><meta charset= \
						\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
						<title>Document</title></head> \
						<body>houhou</body></html>\n\n"

#define TIMEOUT_RESPONSE	3

class Server;

class ResponseHTTP
{
	private:
		map<string, string>		_static_ext_map;
		map<uint32_t, string>	_static_code;
		string			_response_string;
		stringstream	_response;
		t_error			_error;
		t_server	_serv;
		Server	*_server;
		char	**_env;
		char	**_arg;
		pid_t	_pid;
		int		_pipefd[2];
		int		_fd[2];
		string	_mime_type;
		string	_uri;
		string	_html;
		string	_header;
		string	_body;
		string	_output;
		bool	_send_mode;
		int		_write_count;
		int		_size_left;
		bool	_need_cgi;
		t_time	_time;
		RequestParser	_request;
		t_server		*_server_config;
		t_location		_location_config;
		bool			_no_location;
		bool			method_allowed(string method);
		bool			redir_is_set(void);
		void			handle_dir(string &uri);
		bool			set_location();
		bool			select_location();
		bool			check_errors();
		void			construct_response();
		void			construct_error_no_config();
		void			create_get_post_response(string);
		void			generate_response_string();
		void			create_dir_page(string uri, map<string, string> files_in_dir);
		char			**create_env(string uri);
		int				handle_cgi_request(string uri);

	public:
		ResponseHTTP(RequestParser &, t_server *, t_error, Server *);
		ResponseHTTP(ResponseHTTP const &);
		~ResponseHTTP();
		ResponseHTTP	&operator=(ResponseHTTP const &resp);
		string	get_response_string(void) const;
		bool	get_need_cgi(void) const;
		void	set_need_cgi(bool);
		void	post_methods();
		void	delete_methods();
		void	construct_html(uint32_t, string &);
		void	generate_400_error(t_error);
		int 	fork_cgi(void);
		int 	write_cgi(void);
		int		read_cgi(void);
		int		get_write(void) const;
		int		get_read(void) const;
		bool	get_send_mode(void) const;
		void	set_send_mode(bool);
		t_error get_error(void) const;
		bool	check_timeout(void);
		void	deactivate_timeout(void);
		void	kill_child(void) const;
		void	_delete_env(void);
		void	close_pipes(void);
};
