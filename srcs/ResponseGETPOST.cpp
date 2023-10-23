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
	{
		perror("opendir");
		_error = error_500;
		return ;
	}
	errno = 0;
	while (1)
	{
		string	name;
		ent = readdir(dir);
		if (errno != 0)
		{
			perror("readdir");
			_error = error_500;
			if (closedir(dir) == -1)
			{
				perror("closedir");
				return ;
			}
			return ;
		}
		if (ent == NULL)
			break ;
		name = ent->d_name;
		if (stat((uri + '/' + ent->d_name).c_str(), &stats) == 0)
		{
			if (S_ISDIR(stats.st_mode))
				name += '/';
		}
		else
			perror("stat");
		files_in_dir.insert(pair<string, string>(name, ""));
	}
	if (closedir(dir) == -1)
	{
		perror("closedir");
		_error = error_500;
		return ;
	}
	if (_location_config.directory_listing)
	{
		create_dir_page(_request.get_uri(), files_in_dir);
		return ;
	}
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

void	ResponseHTTP::create_get_post_response(string method)
{
	struct stat	stats;
	string		root("./");
	string		uri;

	_error = no_error_200;
	select_location();
	if (!method_allowed(method))
		return (generate_response_string());
	if (redir_is_set())
		return (generate_response_string());
	root += _location_config.root + '/';
	size_t	pos = _request.get_uri().find(_location_config.uri);
	if (pos == 0)
		uri = get_path(_request.get_uri(), _location_config);
	if (stat(uri.c_str(), &stats) == 0)
	{
		if (S_ISDIR(stats.st_mode))
		{
			handle_dir(uri);
			if (_error != no_error_200)
				return (generate_response_string());
		}
		if (stat(uri.c_str(), &stats) == 0)
		{
			if (S_ISREG(stats.st_mode) && stats.st_size != 0 && stats.st_size < INT_MAX)
			{
				ifstream	file;

				pos = uri.find_last_of('.');
				string	ext = uri.substr(pos, uri.size() - pos);
				if (ext == ".php")
				{
					string file_location = this->_location_config.root + "/" + this->_location_config.file_location;
					handle_cgi_request(uri, file_location);
					return ;
				}
				file.open(uri.c_str());
				if (!file.fail())
				{
					pos = uri.find_last_of('.');
					string	ext = uri.substr(pos, uri.size() - pos);
					if (_static_ext_map.find(ext) != _static_ext_map.end())
					{
						_mime_type = _static_ext_map.find(ext)->second;
					}
					stringstream buffer;
					buffer << file.rdbuf();
					_html = buffer.str();
					return (generate_response_string());
				}
			}
			else if (stats.st_size == 0)
				return (generate_response_string());
			else if (stats.st_size >= INT_MAX)
			{
				_error = error_500;
				return (generate_response_string());
			}
		}
		else
		{
			perror("stat");
			_error = error_500;
		}
	}
	_error = error_404;
	return (generate_response_string());
}

void ResponseHTTP::delete_methods()
{
	select_location();
	string path = get_path(_request.get_uri(), _location_config);

	if (!isFile(path) && !isDir(path))
		return (generate_400_error(error_400));
	if (!isWrite(path))
		return (generate_400_error(error_404));
	if (remove(path.c_str()) != 0)
		return (generate_400_error(error_500));
	return (generate_400_error(no_error_200));
}
