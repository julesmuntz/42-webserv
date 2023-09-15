#pragma once

#include "RequestParser.hpp"
#include "Server.hpp"
#include <iostream>
#include <string>
#include <map>
#include <stdint.h>
// typedef struct s_responce
// {
// 	std::string	status;
// 	std::string	header;
// 	std::string	body;
// }		t_responce;

class ResponseHTTP
{
	private:
		std::map<uint32_t, std::string> _static_code;

		std::string	_responce;
		std::string	_header;
		std::string	_body;

		RequestParser _request;
		std::vector<t_server> _server_config;
		//t_responce	_responce;
		bool		_location;
		void		set_location()
		{
			if (!_request.get_uri().empty())
				_location = true;
		}
	public:
		ResponseHTTP(RequestParser &, std::vector<t_server> &);
		~ResponseHTTP();
		//choisir quel function on vas utiliser pour cree la reponce
			void	setup()
			{
				if (_location)
					return (get());
				else
					return (generate_400_error());
			}
		//creation des error
			void	generate_400_error();
			void	error_400();
		//creation des methods post get delete
			void	get();
			void	post();
};
