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
		pid_t	_pid;
		std::vector<int> _write_read_pipes;
		string	_mime_type;
		string	_html;
		string	_header;
		string	_body;
		bool	_need_cgi;
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
		char			**create_env(RequestParser &rp, string uri, string file_location, t_error error);
		int				handle_cgi_request(RequestParser &rp, string uri, string file_location, t_error error);

	public:
		ResponseHTTP(RequestParser &, t_server *, t_error );
		~ResponseHTTP();
		int		handle_cgi_request(RequestParser &rp, string uri, string file_location, t_error error);
		string	get_response_string(void) const;
		bool	get_need_cgi(void) const;
		int		*get_write_read(void) const;
		void	post_methods();
		void	delete_methods();
		void	construct_html(uint32_t, string &);
		void	generate_4000_error(t_error);
};
