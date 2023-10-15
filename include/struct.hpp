#pragma once

#include <string>
#include <map>
#include <vector>
#include <stdint.h>

using namespace std;

typedef struct s_location
{
	string			uri;
	map<string, string>	allow_methods;
	string			root;
	vector<string>	index;
	string			redir_link;
	string			file_location;
	string			cgi_script;
	bool			directory_listing;
}		t_location;

typedef struct s_server
{
	pair<uint32_t, string>			listen;
	vector<string>					server_name;
	uint32_t						client_body_size;
	map<uint32_t, string>			error_pages;
	vector<t_location>				location;
	bool							cgi;
}		t_server;