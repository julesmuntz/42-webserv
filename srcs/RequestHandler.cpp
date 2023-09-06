#include "Server.hpp"

RequestHandler::RequestHandler()
{
	this->req.method = NONE;
	this->req.body = false;
	this->req.chunked = false;
	this->req.request = "";
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

RequestHandler::~RequestHandler() {}

bool		RequestHandler::request_is_over(void)
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

bool	RequestHandler::add_data(std::string str)
{
	req.request += str;
	std::cout << "Request is: " << req.request << std::endl;
	return (!this->request_is_over());
}
