#include "ResponseHTTP.hpp"
#include <fstream>
#include <dirent.h>
#include "ResponseDir.hpp"

/**********************************************************************************/
/* -------------------------constructeur destructeur----------------------------- */
/**********************************************************************************/

static map<uint32_t, string> generate_static_code()
{
	map<uint32_t, string> result;

	result.insert(pair<uint32_t, string>(100, "Continue"));
	result.insert(pair<uint32_t, string>(101, "Switching Protocol"));
	result.insert(pair<uint32_t, string>(102, "Processing"));
	result.insert(pair<uint32_t, string>(103, "Early Hints"));
	result.insert(pair<uint32_t, string>(200, "OK"));
	result.insert(pair<uint32_t, string>(201, "Created"));
	result.insert(pair<uint32_t, string>(202, "Accepted"));
	result.insert(pair<uint32_t, string>(203, "Non-Authoritative Information"));
	result.insert(pair<uint32_t, string>(204, "No Content"));
	result.insert(pair<uint32_t, string>(205, "Reset Content"));
	result.insert(pair<uint32_t, string>(206, "Partial Content"));
	result.insert(pair<uint32_t, string>(207, "Multi-Status"));
	result.insert(pair<uint32_t, string>(208, "Already Reported"));
	result.insert(pair<uint32_t, string>(226, "IM Used"));
	result.insert(pair<uint32_t, string>(300, "Multiple Choices"));
	result.insert(pair<uint32_t, string>(301, "Moved Permanently"));
	result.insert(pair<uint32_t, string>(302, "Found"));
	result.insert(pair<uint32_t, string>(303, "see Other"));
	result.insert(pair<uint32_t, string>(304, "Not Modified"));
	result.insert(pair<uint32_t, string>(307, "Temporary Redirect"));
	result.insert(pair<uint32_t, string>(308, "Permanent Redirect"));
	result.insert(pair<uint32_t, string>(400, "Bad Request"));
	result.insert(pair<uint32_t, string>(401, "Unauthorized"));
	result.insert(pair<uint32_t, string>(402, "Payment Required"));
	result.insert(pair<uint32_t, string>(403, "Forbidden"));
	result.insert(pair<uint32_t, string>(404, "Not Found"));
	result.insert(pair<uint32_t, string>(405, "Method Not Allowed"));
	result.insert(pair<uint32_t, string>(406, "Not Acceptable"));
	result.insert(pair<uint32_t, string>(407, "Proxy Authentication Required"));
	result.insert(pair<uint32_t, string>(408, "Request Timeout"));
	result.insert(pair<uint32_t, string>(409, "Conflict"));
	result.insert(pair<uint32_t, string>(410, "Gone"));
	result.insert(pair<uint32_t, string>(411, "Length Required"));
	result.insert(pair<uint32_t, string>(412, "Precondition Failed"));
	result.insert(pair<uint32_t, string>(413, "Payload Too Large"));
	result.insert(pair<uint32_t, string>(414, "URI Too Long"));
	result.insert(pair<uint32_t, string>(415, "Unsupported Media Type"));
	result.insert(pair<uint32_t, string>(416, "Range Not Satisfiable"));
	result.insert(pair<uint32_t, string>(417, "Expectation Failed"));
	result.insert(pair<uint32_t, string>(418, "I'm a teapot"));
	result.insert(pair<uint32_t, string>(421, "Misdirected Request"));
	result.insert(pair<uint32_t, string>(422, "Unprocessable Entity"));
	result.insert(pair<uint32_t, string>(423, "Locked"));
	result.insert(pair<uint32_t, string>(424, "Failed Dependency"));
	result.insert(pair<uint32_t, string>(425, "Too Early"));
	result.insert(pair<uint32_t, string>(426, "Upgrade Required"));
	result.insert(pair<uint32_t, string>(428, "Precondition Required"));
	result.insert(pair<uint32_t, string>(429, "Too Many Requests"));
	result.insert(pair<uint32_t, string>(431, "Request Header Fields Too Large"));
	result.insert(pair<uint32_t, string>(451, "Unavailable For Legal Reasons"));
	result.insert(pair<uint32_t, string>(500, "Internal Server Error"));
	result.insert(pair<uint32_t, string>(501, "Not Implemented"));
	result.insert(pair<uint32_t, string>(502, "Bad Gateway"));
	result.insert(pair<uint32_t, string>(503, "Service Unavailable"));
	result.insert(pair<uint32_t, string>(504, "Gateway Timeout"));
	result.insert(pair<uint32_t, string>(505, "HTTP Version Not Supported"));
	result.insert(pair<uint32_t, string>(506, "Variant Also Negotiates"));
	result.insert(pair<uint32_t, string>(507, "Insufficient Storage"));
	result.insert(pair<uint32_t, string>(508, "Loop Detected"));
	result.insert(pair<uint32_t, string>(510, "Not Extended"));
	result.insert(pair<uint32_t, string>(511, "Network Authentication Required"));

	return (result);
}

// ResponseHTTP::ResponseHTTP(t_server, t_server server_config)
// {
// 	this->_static_code = generate_static_code();
// }

/*
	Make a factory for the different methods we handle?
		GET
		POST
		DELETE
	They have common fonctionalities, but different methods
	Do we really need this?
	Try and see...
*/

ResponseHTTP::ResponseHTTP(RequestParser &request, t_error error)
{
	this->_static_code = generate_static_code();
	this->_request = request;
	this->_no_location = this->set_location();
	this->_error = error;
	this->generate_4000_error(error);
}

ResponseHTTP::ResponseHTTP(RequestParser &request, t_server server_config, t_error error)
{
	this->_static_code = generate_static_code();
	this->_request = request;
	this->_server_config = server_config;
	this->_no_location = this->set_location();
	this->_error = error;
	this->construct_response();
}

ResponseHTTP::~ResponseHTTP(){}

bool	ResponseHTTP::set_location()
{
	if (!_request.get_uri().empty())
		return (true);
	for (vector<t_location>::iterator it = _server_config.location.begin(); it != _server_config.location.end(); it++)
	{
		if (it->file_location == _request.get_uri())
			return (false);
	}
	return (true);
}

/**********************************************************************************/
/* ----------------------------generate response--------------------------------- */
/**********************************************************************************/

void	ResponseHTTP::generate_response_string()
{
	map<uint32_t, string>::iterator it = _static_code.find(_error);

	if (it != _static_code.end())
	{
		if (it->first != 301 && it->first != 200)
			return (generate_4000_error(_error));
		_response << "HTTP/1.1 " << it->first << " " << it->second << "\r\n";
		if (it->first == 301)
		{
			_response << "Location: " << _location_config.redir_link << "\r\n";
			_response_string = _response.str();
			return ;
		}
		_response << "Content-Type: text/html\r\n";
		_response << "Content-Length: " << _html.length() << "\r\n";
		_response << "\r\n";
		_response << _html;
		_response_string = _response.str();
	}
}

// check this function again cause not sure
// void	ResponseHTTP::generate_400_error()
// {
// 	map<uint32_t, string>::iterator it = _static_code.find(_error);

// 	if (it != _static_code.end())
// 	{
// 		_header = ERRORHEAD;
// 		_body = ERRORBODY_PART_1;
// 		stringstream num;
// 		num << it->first;
// 		_body += num.str();
// 		_body += ERRORBODY_PART_2;
// 		_body += it->second;
// 		_body += ERRORBODY_PART_3;
// 		_html = _header + _body;
// 		if (_server_config.error_pages.find(_error) != _server_config.error_pages.end())
// 		{
// 			ifstream	file;
// 			string		filename = "." + _server_config.error_pages.find(_error)->second;
// 			file.open(filename.c_str());
// 			if (!file.fail())
// 			{
// 				stringstream buffer;
// 				buffer << file.rdbuf();
// 				_html = buffer.str();
// 			}
// 		}
// 		_response << "HTTP/1.1 " << it->first << " " << it->second << "\r\n";
// 		_response << "Content-Type: text/html\r\n";
// 		_response << "Content-Length: " << _html.length() << "\r\n";
// 		_response << "\r\n";
// 		_response << _html;
// 		_response_string = _response.str();
// 	}
// }

/**********************************************************************************/
/* ---------------------------------check_errors--------------------------------- */
/**********************************************************************************/

//more errors to add
bool	ResponseHTTP::check_errors()
{
	if (_error)
		return (true);
	if (_request.get_req_head().hosts.first.empty())
		_error = error_400;
	if (!_request.get_rep_head().transfer_encoding.empty()
			&& _request.get_rep_head().transfer_encoding != "chunked")
		_error = error_400;
	if (_request.get_body().size() > _server_config.client_body_size)
		_error = error_431;
	if (_error)
		return (true);
	return (false);
}

// void	ResponseHTTP::construct_error_no_config()
// {
// 	if (!_error)
// 		_error = error_400;
// 	map<uint32_t, string>::iterator it = _static_code.find(_error);

// 	if (it != _static_code.end())
// 	{
// 		_header = ERRORHEAD;
// 		_body = ERRORBODY_PART_1;
// 		stringstream	num;
// 		num << it->first;
// 		_body += num.str();
// 		_body += ERRORBODY_PART_2;
// 		_body += it->second;
// 		_body += ERRORBODY_PART_3;
// 		_html = _header + _body;
// 		_response << "HTTP/1.1 " << it->first << " " << it->second << "\r\n";
// 		_response << "Content-Type: text/html\r\n";
// 		_response << "Content-Length: " << _html.length() << "\r\n";
// 		_response << "\r\n";
// 		_response << _html;
// 		_response_string = _response.str();
// 	}
// }


void	ResponseHTTP::select_location()
{
	vector<t_location>::iterator	it;
	//order locations by size then try each one

	sort(_server_config.location.begin(), _server_config.location.end(), cmp);
	for (it = _server_config.location.begin(); it != _server_config.location.end(); it++)
	{
		if (it->uri == _request.get_uri().substr(0, it->uri.size()))
			_location_config = *it;
	}
}

void	ResponseHTTP::create_dir_page(string uri, map<string, string> files_in_dir)
{
	_html =  BEFORE_A_HREF;
	for (map<string, string>::iterator it = files_in_dir.begin(); it != files_in_dir.end(); it++)
	{
		string	a_href;
		a_href = "<a href='";
		a_href += uri + it->first;
		a_href += "'>";
		a_href += it->first;
		a_href += "</a>";
		_html += a_href;
	}
	_html += AFTER_A_HREF;
}

void	ResponseHTTP::create_get_response()
{
	string	root(".");

	if (_error == error_301)
		_error = error_304;
	else
		_error = no_error_200;
	select_location();
	//allow methods
	if (_location_config.allow_methods.find("GET") == _location_config.allow_methods.end())
	{
		_error = error_405;
		std::cout << "\e[32m" << _error << "\e[0m GET" << std::endl;//temp
		return ;
	}
	//redir_link
	if (!_location_config.redir_link.empty())
	{
		_error = error_301;
		_html = "";
		std::cout << "\e[32m" << _error << "\e[0m GET" << std::endl;//temp
		return ;
	}
	//directory
	struct stat	stats;
	root += _location_config.root;
	root += '/';
	string uri;
	size_t	pos = _request.get_uri().find(_location_config.uri);
	if (pos == 0)
	{
		uri = _request.get_uri().replace(0, _location_config.uri.size(), root);
	}
	if (stat(uri.c_str(), &stats) == 0)
	{
		if (S_ISDIR(stats.st_mode))
		{
			struct dirent		*ent;
			map<string, string>	files_in_dir;
			// is a directory and work on it
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
			// loop through indexes and loop through content to find match
			for (vector<string>::iterator it = _location_config.index.begin(); it != _location_config.index.end(); it++)
			{
				// if found, break, put it in new uri
				if (files_in_dir.find(*it) != files_in_dir.end())
				{
					uri += '/';
					uri += *it;
					break ;
				}
			}
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

void	ResponseHTTP::create_post_response()
{
	_error = no_error_200;
	select_location();
	if (_location_config.allow_methods.find("POST") == _location_config.allow_methods.end() || _request.get_uri() == "/")
	{
		_error = error_405;
		std::cout << "\e[32m" << _error << "\e[0m POST" << std::endl;//temp
		return ;
	}
	string root(".");
	root += _location_config.root;
	root += '/';
	string uri;
	size_t pos = _request.get_uri().find(_location_config.uri);
	if (pos == 0)
		uri = _request.get_uri().replace(0, _location_config.uri.size(), root);
	struct stat stats;
	if (stat(uri.c_str(), &stats) == 0)
	{
		if (S_ISDIR(stats.st_mode))
		{
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
			if (_location_config.directory_listing)
			{
				create_dir_page(_request.get_uri(), files_in_dir);
				std::cout << "\e[32m" << _error << "\e[0m POST" << std::endl;//temp
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
					_error = error_301;
					std::cout << "\e[32m" << _error << "\e[0m POST" << std::endl;//temp
					create_get_response();
					return ;
				}
			}
		}
	}
	_error = error_404;
}

/**********************************************************************************/
/* ----------------------------------setup--------------------------------------- */
/**********************************************************************************/

void ResponseHTTP::construct_response()
{
	this->_response_string = DUMMY_RESPONSE;
	if (check_errors())
		return (generate_4000_error(_error));
	if (_request.get_method() == "DELETE")
		return (delete_methods());
		// different function or class dep//	verifier les droit d'ecritureending on the request method, could be cgi
		// [GET] // a map of functions ?
		// for now, dummy response
	if (_request.get_method() == "GET")
	{
		create_get_response();
	}
	if (_request.get_method() == "POST")
	{
		create_post_response();
	}
	generate_response_string();
}

string ResponseHTTP::get_response_string(void) const
{
	return (_response_string);
}

void ResponseHTTP::delete_methods()
{
	string path = _request.get_uri();
	select_location();
	path.replace(0, _location_config.uri.size(), _location_config.root);

	if (!isFile(path) && !isDir(path))
		return (generate_4000_error(error_400));
	if (!isWrite(path))
		return (generate_4000_error(error_404));
	if (remove(path.c_str()) != 0)
		return (generate_4000_error(error_500));
	return (generate_4000_error(no_error_200));
}

// check error general

// fill structure error, si erreur, alors
// page generee

// Edouard clean fucntion generate page error or not

void ResponseHTTP::construct_html(uint32_t code, string &str_code)
{
	stringstream num;
	_header = ERRORHEAD;
	_body = ERRORBODY_PART_1;
	num << code;
	_body += num.str();
	_body += ERRORBODY_PART_2;
	_body += str_code;
	_body += ERRORBODY_PART_3;
	_html = _header + _body;
}

void ResponseHTTP::generate_4000_error(t_error error)
{
	if (!error)
		error = error_400;
	map<uint32_t, string>::iterator it = _static_code.find(error);
	if (it != _static_code.end())
	{
		this->construct_html(it->first, it->second);
		if (_server_config.error_pages.find(_error) != _server_config.error_pages.end())
		{
			ifstream file;
			string filename = "." + _server_config.error_pages.find(_error)->second;
			file.open(filename.c_str());
			if (!file.fail())
			{
				stringstream buffer;
				buffer << file.rdbuf();
				_html = buffer.str();
			}
		}
	}
	_response << "HTTP/1.1 " << it->first << " " << it->second << "\r\n";
	_response << "Content-Type: text/html\r\n";
	_response << "Content-Length: " << _html.length() << "\r\n";
	_response << "\r\n";
	_response << _html;
	_response_string = _response.str();
}