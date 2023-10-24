#include "ResponseHTTP.hpp"


char **ResponseHTTP::create_env(string uri)
{
	std::vector<std::string> env_vars;
	env_vars.push_back("REQUEST_METHOD=" + _request.get_method());
	env_vars.push_back("CONTENT_LENGTH=" + _request.get_rep_head().content_length);
	env_vars.push_back("CONTENT_TYPE=" + _request.get_rep_head().content_type);
	env_vars.push_back("SCRIPT_FILENAME=" + uri);
	env_vars.push_back("REMOTE_ADDR=" + string("127.0.0.1"));
	env_vars.push_back("REMOTE_PORT=" + string("80"));
	env_vars.push_back("SERVER_SOFTWARE=" + string("WebServ/1.42"));
	env_vars.push_back("SERVER_NAME=" + _request.get_req_head().hosts.first);
	env_vars.push_back("SERVER_PORT=" + uint32_to_string(_request.get_req_head().hosts.second));
	env_vars.push_back("SERVER_PROTOCOL=" + string("HTTP/1.1"));
	env_vars.push_back("HTTP_USER_AGENT=" + _request.get_req_head().user_agent);
	stringstream ss_error;
	ss_error << _error;
	string s_error;
	ss_error >> s_error;
	env_vars.push_back("REDIRECT_STATUS=" + s_error);
	env_vars.push_back("PATH_INFO=" + _location_config.root);
	char **env = new char *[env_vars.size() + 1];
	for (std::size_t i = 0; i < env_vars.size(); ++i)
	{
		// std::cout << env_vars[i] << std::endl;
		env[i] = new char[env_vars[i].length() + 1];
		std::strcpy(env[i], env_vars[i].c_str());
	}
	env[env_vars.size()] = NULL;
	return env;
}

int ResponseHTTP::handle_cgi_request(string uri)
{
	_uri = uri;
	_env = create_env(uri);
	_need_cgi = true;
	char *arg[] = {const_cast<char *>("cgi-bin/php-cgi"), const_cast<char *>(uri.c_str()), NULL};
	_arg = arg;
	if (pipe(_pipefd) == -1)
	{
		perror("pipe");
		return 1;
	}
	if (pipe(_fd) == -1)
	{
		perror("pipe");
		return 1;
	}
	return 0;
}

int ResponseHTTP::write_cgi()
{
	int			size_to_send;

	if (_pid == 2)
	{
		if (fork_cgi() == 2)
			return (2);
	}
	if (_pid != 0)
	{
		if (_size_left < 1024)
			size_to_send = _size_left;
		else
			size_to_send = 1024;
		if (_size_left == 0)
		{
			return (1);
		}
		int n = write(_fd[1], _request.get_body().c_str() + _write_count, size_to_send);
		_write_count += size_to_send;
		_size_left -= size_to_send;
		if (n <= 0)
		{
			return (1);
		}
	}
	return (0);
}

int ResponseHTTP::fork_cgi()
{
	char *arg[] = {const_cast<char *>("/bin/php-cgi"), const_cast<char *>(_uri.c_str()), NULL};
	// char *argTester[] = {const_cast<char *>("ubuntu_cgi_tester"), NULL};
	_pid = fork();
	if (_pid == -1)
	{
		perror("fork");
		if (_pipefd[0] != -1)
			close(_pipefd[0]);
		if (_fd[0] != -1)
			close(_fd[0]);
		if (_fd[1] != -1)
			close(_fd[1]);
		return 1;
	}
	if (_pid == 0)
	{
		if (dup2(_fd[0], STDIN_FILENO) == -1)
		{
			perror("dup2");
			delete_env(_env, 14);
			return (2);
		}
		if (dup2(_pipefd[1], STDOUT_FILENO) == -1)
		{
			perror ("dup2");
			delete_env(_env, 14);
			return (2);
		}
		if (_pipefd[1] != -1)
			close(_pipefd[1]);
		if (_pipefd[0] != -1)
			close(_pipefd[0]);
		if (_fd[0] != -1)
			close(_fd[0]);
		if (_fd[1] != -1)
			close(_fd[1]);
		_server->shutdown_server();
		execve(arg[0], arg, _env);
		perror("execve");
		// execve(argTester[0], argTester, _env);
		// perror("execve");
		delete_env(_env, 14);
		return (2);
	}
	return 0;
}

int ResponseHTTP::read_cgi()
{
	static int	i;

	int	n = 0;
	if (i == 0)
	{
		if (_pipefd[1] != -1)
			close(_pipefd[1]);
	}
	i = 1;
	char	buffer[10000];
	if (_pipefd[0] != -1)
		n = read(_pipefd[0], buffer, 9999);
	if (n >= 0)
		buffer[n] = 0;
	std::string	addon;
	addon.assign(buffer, n);
	_output += addon;
	if (n <= 0)
	{
		string mime_type = _output.erase(0, _output.find("\r\n\r\n") + 4);
		if (mime_type.find("Content-Length: ") != string::npos)
			_mime_type = mime_type.erase(mime_type.find("Content-Length: ") + 16, mime_type.find("\r\n") + 2);
		_html = _output;
		if (_pipefd[0] != -1)
			close(_pipefd[0]);
		if (_fd[0] != -1)
			close(_fd[0]);
		if (_fd[1] != -1)
			close(_fd[1]);
		int status;
		waitpid(_pid, &status, 0);
		delete_env(_env, 14);
		if (WIFSIGNALED(status) && WTERMSIG(status) == 2)
		{
			std::cout << "SIGNAL" << std::endl;
			//return error
		}
		generate_response_string();
		_need_cgi = false;
		i = 0;
		return 1;
	}
	return 0;
}
