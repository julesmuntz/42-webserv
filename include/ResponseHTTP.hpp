#pragma once

#include "ResponseError.hpp"
#include "RequestParser.hpp"
#include "Server.hpp"

class ResponseHTTP
{
	private:
		map<uint32_t, string> _static_code;
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
		ResponseHTTP(RequestParser &, t_server &);
		~ResponseHTTP();
		//choisir quel function on vas utiliser pour cree la reponce
		void	setup();
		//creation des error
		void	generate_400_error(int code);
		//creation des methods post get delete
		void	get_methods();
		void	post_methods();
		//faire les check d'ereur posible too long ...

};
