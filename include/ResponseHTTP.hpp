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
		map<uint32_t, string>	_static_code;
		string			_response_string;
		stringstream	_response;
		t_error			_error;
		// int		_status;
		string	_html;
		string	_header;
		string	_body;
		RequestParser	_request;
		t_server		_server_config;
		t_location		_location_config;
		bool			_no_location;
		bool			set_location();
		void			select_location();
		bool			check_errors();
		//choisir quel function on vas utiliser pour cree la reponse
		void			construct_response();
		void			construct_error_no_config();
		void			create_get_response();
		void			create_post_response();
		void			generate_response_string();
		void			create_dir_page(string uri, map<string, string> files_in_dir);

	public:
		ResponseHTTP(RequestParser &request, t_error error);
		ResponseHTTP(RequestParser &, t_server , t_error );
		~ResponseHTTP();
		string	get_response_string(void) const;
		//creation des error
		void	generate_400_error();
		//creation des methods post get delete
		//void	get_methods();
		void	post_methods();
		void	delete_methods();
		//faire les check d'ereur posible too long ...



		void	construct_html(uint32_t, string &);
		void	generate_4000_error(t_error);
};
