#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <vector>


typedef struct s_Request_headers
{
	std::string	host;
	std::string	user_agent;
	std::string	accept;
	std::string	accept_language;
	std::string	accept_encoding;
}				t_Request_headers;

typedef struct s_General_headers
{
	std::string	connection;
	std::string	upgrade_insecure_requests;
}				t_General_headers;

typedef struct s_Represent_headers
{
	std::string	content_type;
	std::string	content_length;
}				t_Represent_headers;

class RequestParser
{
	private:

		std::vector<std::string> _lines;

		std::string	_request;

		std::string	_methods;
		std::string	_uri;
		std::string	_version;
		t_Request_headers	_req_head;
		t_General_headers	_gen_head;
		t_Represent_headers	_rep_head;

		void	set_muv();
		void	set_request_header();
		void	set_general_header();
		void	set_represent_header();

		void	set_host(std::string, std::string);
		void	set_user_agent(std::string, std::string);
		void	set_accept(std::string, std::string);
		void	set_accept_language(std::string, std::string);
		void	set_accept_encoding(std::string, std::string);

		void	set_connection(std::string, std::string);
		void	set_upgrade_insecure_requests(std::string, std::string);

		void	set_content_type(std::string, std::string);
		void	set_content_length(std::string, std::string);

	public:
		RequestParser();
		RequestParser(std::string);
		~RequestParser();

		RequestParser	&operator=(const RequestParser &);

		std::string	get_methods() const;
		std::string	get_uri() const;
		std::string	get_version() const;
		t_Request_headers	get_req_head() const;
		t_General_headers	get_gen_head() const;
		t_Represent_headers	get_rep_head() const;

};
