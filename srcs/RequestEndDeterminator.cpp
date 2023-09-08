#include "Server.hpp"

//implement timeout of client requests (timeout of connection)
//implement request too long errors
//how to implement timeout ?

/* check everytime we enter the loop if the time has gone
past what is necessary everytime we go through epoll_wait I guess */

RequestEndDeterminator::RequestEndDeterminator()
{
	time_t	ref;

	time(&ref);
	this->req.method = NONE;
	this->req.body = false;
	this->req.chunked = false;
	this->req.time.active = true;
	this->req.time.timeout = false;
	this->req.time.start_time = ref;
	this->req.time.time_passed_since = 0;
	this->req.request = "";
}

RequestEndDeterminator::~RequestEndDeterminator() {}

void	RequestEndDeterminator::deactivate_timeout(void)
{
	this->req.time.active = false;
}

bool	RequestEndDeterminator::check_timeout()
{
	time_t	now;

	if (this->req.time.active == false)
		return (false);
	time(&now);
	this->req.time.time_passed_since = difftime(now, this->req.time.start_time);
	if (this->req.time.time_passed_since > TIMEOUT)
	{
		this->req.time.timeout = true;
		return (true);
	}
	return (false);
}

void	get_request_method(t_request &req)
{
	std::istringstream	request(req.request);
	std::string			first_line;
	std::string			method_word;

	std::getline(request, first_line);
	if (*first_line.rbegin() == '\r')
	{
		first_line.erase(first_line.size() - 1);
		std::istringstream	first_line_stream(first_line);
		first_line_stream >> method_word;
		if (method_word == "GET")
			req.method = GET;
		else if (method_word == "DELETE")
			req.method = DELETE;
		else if (method_word == "POST")
			req.method = POST;
		else
			req.method = ERROR;
	}
	else
	{
		req.method = NONE;
	}
}

bool	req_is_chunked(t_request &req)
{
	if (req.request.find("\r\nTransfer-Encoding: chunked\r\n") != std::string::npos)
	{
		req.chunked = true;
		return (true);
	}
	return (false);
}

bool	req_has_body(t_request &req)
{
	if (req.request.find("\r\n\r\n") != std::string::npos)
	{
		req.body = true;
		return (true);
	}
	return (false);
}

bool	this_is_the_end(t_request &req)
{
	if (req.chunked)
	{
		if (req.request.find("\r\n0\r\n\r\n") != std::string::npos)
			return (true);
	}
	else if (req.method != POST)
	{
		if (req.request.find("\r\n\r\n") != std::string::npos)
			return (true);
	}
	else
	{
		if (req.body)
		{
			size_t	pos = req.request.find("\r\n\r\n");
			if (req.request.find("\r\n\r\n", pos + 1) != std::string::npos)
				return (true);
		}
	}
	return (false);
}

bool	RequestEndDeterminator::request_is_over(void)
{
	std::cout << std::endl << "Is the request over?" << std::endl << std::endl;
	if (req.method == NONE)
	{
		get_request_method(req);
		std::cout << "Request method is: " << req.method << std::endl << std::endl;
	}
	if (req.method == GET || req.method == DELETE)
	{
		if (this_is_the_end(req))
			return (true);
		return (false);
	}
	if (req.method == POST && !req.chunked)
	{
		if (req.body)
		{
			if (this_is_the_end(req))
				return (true);
			return (false);
		}
		if (req_is_chunked(req))
		{
			if (this_is_the_end(req))
				return (true);
			return (false);
		}
		else
		{
			if (req_has_body(req))
			{
				if (this_is_the_end(req))
					return (true);
				return (false);
			}
		}
	}
	if (req.chunked)
	{
		if (this_is_the_end(req))
			return (true);
		return (false);
	}
	return (false);
}
