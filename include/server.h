#include <iostream>
#include <vector>
//#include <string>
#define FILE_CONF ".conf"
#define ERROR_FILENAME "config file does not end with .conf"
#define ERROR_OPEN "erro open"

typedef struct s_location
{
	/*tout ce qui peut etre dans location*/
	std::string				allow_methods;
	std::string				root;
	std::string				index;
	std::string				cgi_pass;
	std::string				client_body_buffer_size;
	bool					alias;
}		t_location;


typedef struct s_server
{
	/*tout ce qui peut etre dans server*/
	std::string				listen;
	std::string				server_name;
	std::string				client_body_buffer_size;
	std::string				error_page;
	std::string				allow_methods;
	std::string				root;
	std::string				link_location;
	std::vector<t_location> location;	
}		t_server;


