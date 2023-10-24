#pragma once

#include "ResponseError.hpp"
#include "Server.hpp"

#define DUMMY_RESPONSE	"HTTP/1.1 200 OK\n\n<!DOCTYPE html><html lang=\"en\"><head><meta charset= \
						\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
						<title>Document</title></head> \
						<body>houhou</body></html>\n\n"

class ResponseHTTP
{
	private:
		map<string, string>		_static_ext_map;
		map<uint32_t, string>	_static_code;
		string			_response_string;
		stringstream	_response;
		t_error			_error;
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
		bool	_need_cgi;
		bool	_send_mode;
		int		_write_count;
		int		_size_left;
		RequestParser	_request;
		t_server		*_server_config;
		t_location		_location_config;
		bool			_no_location;
		bool			method_allowed(string method);
		bool			redir_is_set(void);
		void			handle_dir(string &uri);
		bool			set_location();
		void			select_location();
		bool			check_errors();
		void			construct_response();
		void			construct_error_no_config();
		void			create_get_response();
		void			create_post_response();
		void			generate_response_string();
		void			create_dir_page(string uri, map<string, string> files_in_dir);
		char			**create_env(string uri, string file_location, t_error error);
		int				handle_cgi_request(string uri, string file_location, t_error error);

	public:
		ResponseHTTP();
		ResponseHTTP(RequestParser &, t_server *, t_error );
		ResponseHTTP(ResponseHTTP const &);
		~ResponseHTTP();
		ResponseHTTP	&operator=(ResponseHTTP const &resp);
		int		handle_cgi_request(RequestParser &rp, string uri, string file_location, t_error error);
		string	get_response_string(void) const;
		bool	get_need_cgi(void) const;
		void	post_methods();
		void	delete_methods();
		void	construct_html(uint32_t, string &);
		void	generate_4000_error(t_error);
		int 	fork_cgi(void);
		int 	write_cgi(void);
		int		read_cgi(void);
		int		get_write(void) const;
		int		get_read(void) const;
		void	set_send_mode(bool send_mode);
		bool	get_send_mode(void) const;
};
