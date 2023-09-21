#pragma once

#include "Server.hpp"
#include "utils.hpp"
#include <fstream>
#include <sys/stat.h>

using namespace std;

class Exodus
{
private:
	ifstream			_ifs;
	vector<t_server>	_server;

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
	void	cgi_pass(t_location *, string);
	void	directory_listing(t_location *, string);

	t_server	set_server();
	t_location	set_location(string);

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
