#include "ResponseHTTP.hpp"
#include <fstream>
#include <dirent.h>
#include "ResponseDir.hpp"

/**********************************************************************************/
/* -------------------------constructeur destructeur----------------------------- */
/**********************************************************************************/

static map<string, string> generate_static_ext_map()
{
	map<string, string> ext_map;

	ext_map.insert(pair<string, string>(".html", "text/html"));
	ext_map.insert(pair<string, string>(".css", "text/css"));
	ext_map.insert(pair<string, string>(".js", "text/javascript"));
	ext_map.insert(pair<string, string>(".htm", "text/html"));
	ext_map.insert(pair<string, string>(".aac", "audio/aac"));
	ext_map.insert(pair<string, string>(".avif", "image/avif"));
	ext_map.insert(pair<string, string>(".avi", "video/x-msvideo"));
	ext_map.insert(pair<string, string>(".bin", "application/octet-stream"));
	ext_map.insert(pair<string, string>(".bmp", "image/bmp"));
	ext_map.insert(pair<string, string>(".bz", "application/x-bzip"));
	ext_map.insert(pair<string, string>(".csh", "application/x-csh"));
	ext_map.insert(pair<string, string>(".csv", "text/csv"));
	ext_map.insert(pair<string, string>(".doc", "application/msword"));
	ext_map.insert(pair<string, string>(".gz", "application/gzip"));
	ext_map.insert(pair<string, string>(".gif", "image/gif"));
	ext_map.insert(pair<string, string>(".ico", "image/vnd.microsoft.icon"));
	ext_map.insert(pair<string, string>(".ics", "text/calendar"));
	ext_map.insert(pair<string, string>(".jpeg", "image/jpeg"));
	ext_map.insert(pair<string, string>(".jpg", "image/jpeg"));
	ext_map.insert(pair<string, string>(".mjs", "text/javascript"));
	ext_map.insert(pair<string, string>(".mp3", "audio/mpeg"));
	ext_map.insert(pair<string, string>(".mp4", "video/mp4"));
	ext_map.insert(pair<string, string>(".mpeg", "video/mpeg"));
	ext_map.insert(pair<string, string>(".png", "image/png"));
	ext_map.insert(pair<string, string>(".pdf", "application/pdf"));
	ext_map.insert(pair<string, string>(".php", "application/x-httpd-php"));
	ext_map.insert(pair<string, string>(".svg", "image/svg+xml"));
	ext_map.insert(pair<string, string>(".tar", "application/x-tar"));
	ext_map.insert(pair<string, string>(".tif", "image/tiff"));
	ext_map.insert(pair<string, string>(".tiff", "image/tiff"));
	ext_map.insert(pair<string, string>(".ttf", "font/ttf"));
	ext_map.insert(pair<string, string>(".txt", "text/plain"));
	ext_map.insert(pair<string, string>(".wav", "audio/wav"));
	ext_map.insert(pair<string, string>(".weba", "audio/webm"));
	ext_map.insert(pair<string, string>(".webm", "video/webm"));
	ext_map.insert(pair<string, string>(".webp", "image/webp"));
	ext_map.insert(pair<string, string>(".woff", "font/woff"));
	ext_map.insert(pair<string, string>(".woff2", "font/woff2"));
	ext_map.insert(pair<string, string>(".xhtml", "appplication/xhtml+xml"));
	ext_map.insert(pair<string, string>(".xml", "appplication/xml"));
	ext_map.insert(pair<string, string>(".zip", "application/zip"));

	return (ext_map);
}

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

string getResponseCode(t_error error)
{
	switch (error)
	{
	case error_400:
		return "400";
	case error_404:
		return "404";
	case error_403:
		return "403";
	case error_405:
		return "405";
	case error_408:
		return "408";
	case error_413:
		return "413";
	case error_431:
		return "431";
	case error_301:
		return "301";
	case error_304:
		return "304";
	case error_500:
		return "500";
	case error_505:
		return "505";
	case no_error_200:
		return "200";
	case no_error:
		return "200";
	default:
		return "0";
	}
}

char **ResponseHTTP::create_env(RequestParser &rp, string cgi_script, string file_location, vector<t_FileInfo>::const_iterator it, t_error error)
{
	std::vector<std::string> env_vars;
	string query_string = "name=" + it->fileName + "&path=cgi-bin/.tmp&dir=" + file_location;
	env_vars.push_back("QUERY_STRING=" + query_string);
	env_vars.push_back("REQUEST_METHOD=" + rp.get_method());
	env_vars.push_back("CONTENT_LENGTH=" + rp.get_rep_head().content_length);
	env_vars.push_back("CONTENT_TYPE=" + rp.get_rep_head().content_type);
	string scriptName = "cgi-bin/" + cgi_script;
	env_vars.push_back("SCRIPT_FILENAME=" + scriptName);
	env_vars.push_back("REMOTE_ADDR=" + string("127.0.0.1"));
	env_vars.push_back("REMOTE_PORT=" + string("80"));
	env_vars.push_back("SERVER_SOFTWARE=" + string("WebServ/1.42"));
	env_vars.push_back("SERVER_NAME=" + rp.get_req_head().hosts.first);
	env_vars.push_back("SERVER_PORT=" + uint32_to_string(rp.get_req_head().hosts.second));
	env_vars.push_back("SERVER_PROTOCOL=" + string("HTTP/1.1"));
	env_vars.push_back("UPLOAD_DIR=" + file_location);
	env_vars.push_back("HTTP_USER_AGENT=" + rp.get_req_head().user_agent);
	env_vars.push_back("REDIRECT_STATUS=" + getResponseCode(error));
	env_vars.push_back("DOCUMENT_ROOT=" + _location_config.root);

	char **env = new char *[env_vars.size() + 1];
	for (std::size_t i = 0; i < env_vars.size(); ++i)
	{
		cout << env_vars[i] << endl;
		env[i] = new char[env_vars[i].length() + 1];
		std::strcpy(env[i], env_vars[i].c_str());
	}
	env[env_vars.size()] = NULL;
	return env;
}

int ResponseHTTP::handle_cgi_request(RequestParser &rp, string cgi_script, string file_location, vector<t_FileInfo>::const_iterator it, t_error error)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return 1;
	}

	char **env = create_env(rp, cgi_script, file_location, it, error);
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return 1;
	}

	if (pid == 0) // Child process
	{
		close(pipefd[1]); // Close write end of the pipe.

		if (dup2(pipefd[0], STDIN_FILENO) == -1)
		{
			perror("dup2");
			exit(EXIT_FAILURE);
		}

		char *arg[] = {(char *)"/usr/bin/php-cgi", NULL};

		execve(arg[0], arg, env);

		perror("execve"); // execve() only returns on error.
		exit(EXIT_FAILURE);
	}
	else // Parent process
	{
		close(pipefd[0]); // Close read end of the pipe.
		close(pipefd[1]); // Close write end of the pipe.

		delete_env(env, 15);
		wait(NULL); // Wait for child process to finish.
		return 0;
	}
}

ResponseHTTP::ResponseHTTP(RequestParser &request, t_server *server_config, t_error error)
{
	this->_mime_type = "text/html";
	this->_static_ext_map = generate_static_ext_map();
	this->_static_code = generate_static_code();
	this->_request = request;
	this->_server_config = server_config;
	this->_error = error;
	if (_server_config != NULL)
	{
		this->_no_location = this->set_location();
		this->construct_response();
		if (server_config->cgi == true)
		{
			request.parseFile();
			for (vector<t_FileInfo>::const_iterator it = request.get_fileInfo().begin(); it != request.get_fileInfo().end(); it++)
			{
				std::ofstream outfile("cgi-bin/.tmp");
				if (!outfile.is_open())
					break;
				outfile << it->fileContent;
				outfile.close();
				string file_location = this->_location_config.root + "/" + this->_location_config.file_location;
				handle_cgi_request(request, this->_location_config.cgi_script, file_location, it, error);
				remove("cgi-bin/.tmp");
			}
		}
		cout << "\e[32m" << getResponseCode(error) << "\e[0m " << _request.get_method() << endl;
		return;
	}
	this->generate_4000_error(error_400);
}

ResponseHTTP::~ResponseHTTP() {}

bool ResponseHTTP::set_location()
{
	if (!_request.get_uri().empty())
		return (true);
	for (vector<t_location>::iterator it = _server_config->location.begin(); it != _server_config->location.end(); it++)
	{
		if (it->file_location == _request.get_uri())
			return (false);
	}
	return (true);
}

/**********************************************************************************/
/* ----------------------------generate response--------------------------------- */
/**********************************************************************************/

void ResponseHTTP::generate_response_string()
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
			return;
		}
		_response << "Content-Type: " << _mime_type << "\r\n";
		_response << "Content-Length: " << _html.length() << "\r\n";
		_response << "\r\n";
		_response << _html;
		_response_string = _response.str();
	}
}

/**********************************************************************************/
/* ---------------------------------check_errors--------------------------------- */
/**********************************************************************************/

bool ResponseHTTP::check_errors()
{
	if (_error)
		return (true);
	if (_request.get_method() != "GET" && _request.get_method() != "DELETE" && _request.get_method() != "POST")
		_error = error_405;
	else if (_request.get_req_head().hosts.first.empty())
		_error = error_400;
	else if (_request.get_version() != "HTTP/1.1")
		_error = error_505;
	else if (!_request.get_rep_head().transfer_encoding.empty() && _request.get_rep_head().transfer_encoding != "chunked")
		_error = error_400;
	else if (_request.get_body().size() > _server_config->client_body_size)
		_error = error_431;
	if (_error)
		return (true);
	return (false);
}

void ResponseHTTP::select_location()
{
	vector<t_location>::iterator it;

	sort(_server_config->location.begin(), _server_config->location.end(), cmp);
	for (it = _server_config->location.begin(); it != _server_config->location.end(); it++)
	{
		if (it->uri == _request.get_uri().substr(0, it->uri.size()))
			_location_config = *it;
	}
}

void ResponseHTTP::create_dir_page(string uri, map<string, string> files_in_dir)
{
	_html = BEFORE_A_HREF;
	for (map<string, string>::iterator it = files_in_dir.begin(); it != files_in_dir.end(); it++)
	{
		string a_href;
		a_href = "<a href='";
		a_href += uri + it->first;
		a_href += "'>";
		a_href += it->first;
		a_href += "</a>";
		_html += a_href;
	}
	_html += AFTER_A_HREF;
}

void ResponseHTTP::create_post_response()
{
	_error = no_error_200;
	select_location();
	if (!method_allowed("POST"))
		return (generate_response_string());
	if (redir_is_set())
		return (generate_response_string());

	string uri = get_path(_request.get_uri(), _location_config);
	struct stat stats;
	if (stat(uri.c_str(), &stats) == 0)
	{
		if (S_ISDIR(stats.st_mode))
		{
			struct dirent *ent;
			map<string, string> files_in_dir;
			DIR *dir = opendir(uri.c_str());
			if (!dir)
				perror("opendir");
			errno = 0;
			while (1)
			{
				string name;
				ent = readdir(dir);
				if (errno != 0)
					perror("readdir");
				if (ent == NULL)
					break;
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
				return (generate_response_string());
			}
			for (vector<string>::iterator it = _location_config.index.begin(); it != _location_config.index.end(); it++)
			{
				if (files_in_dir.find(*it) != files_in_dir.end())
				{
					uri += '/';
					uri += *it;
					break;
				}
			}
		}
		if (stat(uri.c_str(), &stats) == 0)
		{
			if (S_ISREG(stats.st_mode) && stats.st_size != 0 && stats.st_size != INT_MAX)
			{
				ifstream file;
				file.open(uri.c_str());
				if (!file.fail())
				{
					stringstream buffer;
					buffer << file.rdbuf();
					_html = buffer.str();
					return (generate_response_string());
				}
			}
		}
	}
	_error = error_404;
	return (generate_response_string());
}

/**********************************************************************************/
/* ----------------------------------setup--------------------------------------- */
/**********************************************************************************/

void ResponseHTTP::construct_response()
{
	if (check_errors())
		return (generate_4000_error(_error));
	if (_request.get_method() == "DELETE")
		return (delete_methods());
	if (_request.get_method() == "GET")
		return (create_get_response());
	if (_request.get_method() == "POST")
		return (create_post_response());
	else
		return (generate_4000_error(_error));
}

string ResponseHTTP::get_response_string(void) const
{
	return (_response_string);
}

void ResponseHTTP::delete_methods()
{
	select_location();
	string path = get_path(_request.get_uri(), _location_config);
	;

	if (!isFile(path) && !isDir(path))
		return (generate_4000_error(error_400));
	if (!isWrite(path))
		return (generate_4000_error(error_404));
	if (remove(path.c_str()) != 0)
		return (generate_4000_error(error_500));
	return (generate_4000_error(no_error_200));
}

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
		if (_server_config && _server_config->error_pages.find(_error) != _server_config->error_pages.end())
		{
			ifstream file;
			string filename = "." + _server_config->error_pages.find(_error)->second;
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
