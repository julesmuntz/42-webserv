#pragma once

#include "utils.hpp"
#include <stdint.h>

using namespace std;

typedef struct s_Request_headers
{
	string	host;
	pair<string, uint32_t> hosts;
	string	user_agent;
	string	accept;
	string	accept_language;
	string	accept_encoding;
}				t_Request_headers;

typedef struct s_General_headers
{
	string	connection;
	string	upgrade_insecure_requests;
}				t_General_headers;

typedef struct s_Represent_headers
{
	string	content_type;
	string	content_length;
	string	transfer_encoding;
}				t_Represent_headers;

class RequestParser
{
	private:

		vector<string> _lines;

		string	_request;

		string	_method;
		string	_uri;
		string	_version;
		string	_body;
		t_Request_headers	_req_head;
		t_General_headers	_gen_head;
		t_Represent_headers	_rep_head;

		void	set_muv();
		void	set_request_header();
		void	set_general_header();
		void	set_represent_header();

		void	set_host(string, string);
		void	set_user_agent(string, string);
		void	set_accept(string, string);
		void	set_accept_language(string, string);
		void	set_accept_encoding(string, string);

		void	set_connection(string, string);
		void	set_upgrade_insecure_requests(string, string);

		void	set_content_type(string, string);
		void	set_content_length(string, string);
		void	set_transfer_encoding(string, string);

		void	set_body();

		void	dechunk_body();

	public:
		RequestParser();
		RequestParser(string);
		~RequestParser();

		RequestParser	&operator=(const RequestParser &);

		string	get_method() const;
		string	get_uri() const;
		string	get_version() const;
		string	get_body() const;
		t_Request_headers	get_req_head() const;
		t_General_headers	get_gen_head() const;
		t_Represent_headers	get_rep_head() const;

};
