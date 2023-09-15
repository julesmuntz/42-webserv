#include "Server.hpp"

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
	this->req.msg_too_long = false;
	this->req.content_length = 0;
	this->req.body_size = 0;
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

void	RequestEndDeterminator::get_request_method(void)
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



// edouard
bool	RequestEndDeterminator::req_is_chunked(void)
{
	if (req.request.find("\r\nTransfer-Encoding: chunked\r\n") != std::string::npos)
	{
		req.chunked = true;
		return (true);
	}
	return (false);
}

// bool	RequestEndDeterminator::req_content_length(void)
// {
// 	std::string	length;
// 	size_t		npos;

// 	size_t	pos = req.request.find("\r\nContent-Length: ");
// 	if (pos != std::string::npos)
// 	{
// 		npos = req.request.find("\r\n", pos + 1);
// 		if (npos == std::string::npos)
// 			return (false);
// 		length = req.request.substr(pos + strlen("\r\nContent-Length: "), npos);
// 		std::stringstream sstream(length);
// 		sstream >> req.content_length;
// 		//check error
// 	}
// 	else
// 		return (false);
// 	return (true);
// }

// size_t	get_body_size(std::string request)
// {
// 	//work to do
// 	//do not forget not to use += when dealing with body append because of zeros
// }


bool	RequestEndDeterminator::req_has_body(void)
{
	size_t		pos;
	// std::string	body;

	pos = req.request.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		// if (this->req_content_length())
		// {
		// }
		// body = req.request.substr(pos + 4, req.request.size());
		// req.body_size = body.size();
		req.body = true;
		return (true);
	}
	return (false);
}

bool	RequestEndDeterminator::this_is_the_end(void)
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
			// req.body_size += 
			size_t	pos = req.request.find("\r\n\r\n");
			if (req.request.find("\r\n\r\n", pos + 1) != std::string::npos)
				return (true);
		}
	}
	return (false);
}

// attribute to mark fallthrough
// link to attributes in GCC: https://gcc.gnu.org/onlinedocs/gcc/Statement-Attributes.html

bool	RequestEndDeterminator::request_is_over(void)
{
	switch (req.method)
	{
		case NONE :
			get_request_method();
			__attribute__ ((fallthrough));
		default :
			switch (req.method)
			{
				case GET ... DELETE :
					return (this_is_the_end());
				case POST :
					switch (req.chunked)
					{
						case false :
							if (req.body)
								return (this_is_the_end());
							if (req_is_chunked())
								return (this_is_the_end());
							else if (req_has_body())
								return (this_is_the_end());
							__attribute__ ((fallthrough));
						default :
							if (req.chunked)
								return (this_is_the_end());
							__attribute__ ((fallthrough));
					}
				case ERROR :
					return (this_is_the_end());
				case NONE :
					break ;
			}
	}
	return (false);
}
