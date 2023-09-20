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
		map<uint32_t, string> _static_code;
		string			_response_string;
		stringstream	_response;
		// int		_status;
		string	_html;
		string	_header;
		string	_body;
		RequestParser	_request;
		t_server		_server_config;
		bool			_no_location;
		bool			set_location();
	public:
		ResponseHTTP(RequestParser &, t_server );
		~ResponseHTTP();
		string	get_response_string(void) const;
		//choisir quel function on vas utiliser pour cree la reponce
		void	setup();
		//creation des error
		void	generate_400_error(int code);
		//creation des methods post get delete
		void	get_methods();
		void	post_methods();
		//faire les check d'ereur posible too long ...

};
