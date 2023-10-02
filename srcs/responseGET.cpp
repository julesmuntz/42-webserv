#include "Server.hpp"
#include "ResponseHTTP.hpp"
#include "ResponseDir.hpp"
#include <fstream>
#include <dirent.h>


bool	ResponseHTTP::method_allowed(string method)
{
	if (_location_config.allow_methods.find(method) == _location_config.allow_methods.end())
	{
		_error = error_405;
		std::cout << "\e[32m" << _error << "\e[0m GET" << std::endl;//temp
		return (false);
	}
	return (true);
}

bool	ResponseHTTP::redir_is_set(void)
{
	if (!_location_config.redir_link.empty())
	{
		_error = error_301;
		_html = "";
		std::cout << "\e[32m" << _error << "\e[0m GET" << std::endl;//temp
		return (true);
	}
	return (false);
}

void	ResponseHTTP::handle_dir(string &uri)
{
	struct stat			stats;
	struct dirent		*ent;
	map<string, string>	files_in_dir;

	DIR	*dir = opendir(uri.c_str());
	if (!dir)
		perror("opendir");
	errno = 0;
	while (1)
	{
		string	name;
		ent = readdir(dir);
		if (errno != 0)
					perror("readdir");
				if (ent == NULL)
					break ;
				name = ent->d_name;
				if (stat((uri + '/' + ent->d_name).c_str(), &stats) == 0)
		{
			if (S_ISDIR(stats.st_mode))
				name += '/';
		}
		files_in_dir.insert(pair<string, string>(name, ""));
	}
	if (closedir(dir) == -1)
		perror("closedir");
	// check directory_listing
	if (_location_config.directory_listing)
	{
		create_dir_page(_request.get_uri(), files_in_dir);
		std::cout << "\e[32m" << _error << "\e[0m GET" << std::endl;//temp
		return ;
	}
	// check index
	for (vector<string>::iterator it = _location_config.index.begin(); it != _location_config.index.end(); it++)
	{
		if (files_in_dir.find(*it) != files_in_dir.end())
		{
			uri += '/';
			uri += *it;
			break ;
		}
	}
}

void	ResponseHTTP::create_get_response()
{
	struct stat	stats;
	string		root(".");
	string		uri;

	_error = no_error_200;
	select_location();
	//allow methods
	if (!method_allowed("GET"))
		return ;
	//redir_link
	if (redir_is_set())
		return ;
	//directory
	root += _location_config.root + '/';
	size_t	pos = _request.get_uri().find(_location_config.uri);
	if (pos == 0)
	{
		uri = _request.get_uri().replace(0, _location_config.uri.size(), root);
	}
	if (stat(uri.c_str(), &stats) == 0)
	{
		if (S_ISDIR(stats.st_mode))
		{
			// is a directory and work on it
			handle_dir(uri);
		}
		// check if uri regular file
		// check size of file
		if (stat(uri.c_str(), &stats) == 0)
		{
			if (S_ISREG(stats.st_mode) && stats.st_size != 0 && stats.st_size != INT_MAX)
			{
				ifstream	file;

				file.open(uri.c_str());
				if (!file.fail())
				{
					stringstream buffer;
					buffer << file.rdbuf();
					_html = buffer.str();
					std::cout << "\e[32m" << _error << "\e[0m GET" << std::endl;//temp
					return ;
				}
			}
		}
		// check size of file
		// if seems ok, open it and put it in _html and put 200 OK
	}
	_error = error_404;
	std::cout << "\e[32m" << _error << "\e[0m GET" << std::endl;//temp
	//cgi
}
