#pragma once

#include "struct.hpp"
#include "utils.hpp"
#include <fstream>
#include <sys/stat.h>

#define FILE_CONF ".conf"
#define FILE_DEFAULT "conf/default.conf"
#define ERROR_FILENAME "config file does not end with .conf"
#define ERROR_OPEN "error open"
#define ERROR_END "error end"

using namespace std;

class Exodus
{
private:
	ifstream			_ifs;
	std::istringstream	_ss;
	vector<t_server>	_server;
	bool		file_error;
	void	failure();
	void	listen(t_server *, string);
	void	server_name(t_server *, string);
	void	client_body_size(t_server *, string);
	void	error_page(t_server *, string);

	void	uri(t_location *, string);
	void	allow_methods(t_location *, string);
	void	root(t_location *, string);
	void	index(t_location *, string);
	void	redir_link(t_location *, string);
	void	file_location(t_location *, string);
	void	cgi_path(t_location *, string);
	void	directory_listing(t_location *, string);

	t_server	set_server();
	t_server	set_server_def();
	t_location	set_location(string);
	t_location	set_location_def(string line);

public:
	Exodus(string const);
	~Exodus();
	vector<t_server>	get_server() const;
	void					setup();

	class error_end : public exception
	{
		public:
			virtual const char *what() const throw();
	};
	class error_filename : public exception
	{
		public:
			virtual const char *what() const throw();
	};
	class error_open : public exception
	{
		public:
			virtual const char *what() const throw();
	};
};
