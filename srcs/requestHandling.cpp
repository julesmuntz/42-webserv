#include "Server.hpp"

//We could make a request class, with the fd, the request,
//the first line of the request so that we can only have it once and consult it you know

void	get_request_method(t_request &req)
{
	std::istringstream	request(req.request);
	std::string			first_line;
	std::string			method_word;

	std::getline(request, first_line);
	if (*first_line.rbegin() == '\r')
	{
		//erase the last character then proceed
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
		req.method = ERROR;
	}
}

bool	req_is_chunked(t_request &req)
{
	if (req.request.find("Transfer-Encoding: chunked\r\n") != std::string::npos)
	{
		req.chunked = true;
		return (true);
	}
	return (false);
}

bool	this_is_the_end(t_request &req)
{
	if (req.chunked)
	{
		if (req.request.find("0\r\n\r\n") != std::string::npos)
			return (true);
	}
	else
	{
		if (req.request.find("\r\n\r\n") != std::string::npos)
			return (true);
	}
	return (false);
}

bool	request_is_over(t_request req)
{
	std::cout << std::endl << "Is the request over?" << std::endl << std::endl;
	std::cout << req.request << std::endl << std::endl;

	if (req.method == NONE)
	{
		get_request_method(req);
		std::cout << "Request method is: " << req.method << std::endl << std::endl;
	}
	if (req.method == GET || req.method == DELETE)
	{
		if (this_is_the_end(req))
		{
			std::cout << "OVER" << std::endl << std::endl;
			return (true);
		}
		std::cout << "POP" << std::endl;
		return (false);
	}
	if (req.method == POST && !req.chunked)
	{
		if (req_is_chunked(req))
		{
			if (this_is_the_end(req))
				return (true);
			return (false);
		}
	}
	if (req.chunked)
	{
		if (this_is_the_end(req))
			return (true);
		return (false);
	}
	return (true);
}

//if GET, DEL, ok done
//else if POST, search for the content-length of the encoding-type=chunked
//then do what is necessary
