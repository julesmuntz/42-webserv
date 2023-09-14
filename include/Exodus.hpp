#pragma once

# include "Server.hpp"
# include <iostream>
# include <fstream>
# include <string>
# include <sstream>
# include <algorithm>


class Exodus
{
private:
	std::ifstream			_ifs;
	std::vector<t_server>	_server;

	void	listen(t_server *, std::string);
	void	server_name(t_server *, std::string);
	void	client_body_size(t_server *, std::string);
	void	error_page(t_server *, std::string);

	void	uri(t_location *, std::string);
	void	allow_methods(t_location *, std::string);
	void	root(t_location *, std::string);
	void	index(t_location *, std::string);
	void	redir_link(t_location *, std::string);
	void	file_location(t_location *, std::string);
	void	cgi_pass(t_location *, std::string);
	void	directory_listing(t_location *, std::string);

	t_server	set_server();
	t_location	set_location(std::string);

public:
	Exodus(std::string const);
	~Exodus();
	std::vector<t_server>	get_server() const;
	void					setup();

	class error_end : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
	class error_filename : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
	class error_open : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
};
