#include "server.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Exodus
{
private:
	std::ifstream			_ifs;
	std::vector<t_server>	_server;

	// std::string				_servers[10];
	// void					(Exodus::*_f[])(t_server *, std::string);
	
public:
	Exodus(std::string const);
	~Exodus();

	t_server				set_server();
	std::vector<t_server>	get_server() const;
	t_location				set_location();
	t_location				get_location() const;
	void					set_parss();
	std::vector<t_server> 	get_parss() const;

	void					get_Exodus();
	template<typename T>
	void					listen(T *, std::string);
	template<typename T>
	void					server_name(T *, std::string);
	template<typename T>
	void					client_body_buffer_size(T *, std::string);
	template<typename T>
	void					error_page(T *, std::string);
	template<typename T>
	void					allow_methods(T *, std::string);
	template<typename T>
	void					root(T *, std::string);
	template<typename T>
	void					index(T *, std::string);
	template<typename T>
	void					cgi_pass(T *, std::string);
	template<typename T>
	void					alias(T *, std::string);
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
