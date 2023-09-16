#pragma once

#include "RequestParser.hpp"
#include "Server.hpp"
#include <iostream>
#include <string>
#include <map>
#include <stdint.h>
// typedef struct s_responce
// {
// 	string	status;
// 	string	header;
// 	string	body;
// }		t_responce;

using namespace std;

class ResponseHTTP
{
	private:
		map<uint32_t, string> _static_code;

		string	_responce;
		string	_header;
		string	_body;

		RequestParser _request;
		t_server _server_config;
		//t_responce	_responce;
		bool		_no_location;
		bool		set_location()
		{
			if (!_request.get_uri().empty())
				return (true);
			for (vector<t_location>::iterator it = _server_config.location.begin(); it != _server_config.location.end(); it++)
			{
				if (it->file_location == _request.get_uri())
					return (false);
			}
			return (true);
		}
	public:
		ResponseHTTP(RequestParser &, t_server &);
		~ResponseHTTP();
		//choisir quel function on vas utiliser pour cree la reponce
			void	setup()
			{
				if (_no_location)
					return (generate_400_error());
			}
		//creation des error
			void	generate_400_error();
			void	error_400();
		//creation des methods post get delete
			void	get();
			void	post();
};
