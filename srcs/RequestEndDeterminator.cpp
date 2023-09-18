#include "Server.hpp"
#include "utils.hpp"

RequestEndDeterminator::RequestEndDeterminator() : msg_len(0), chunked_nb_chars(-1), chunked_data_read(""), chunked_pos_in_req(0), bad_request(false)
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
	this->req.header_size = 0;
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
	istringstream	request(req.request);
	string			first_line;
	string			method_word;

	getline(request, first_line);
	if (*first_line.rbegin() == '\r')
	{
		first_line.erase(first_line.size() - 1);
		istringstream	first_line_stream(first_line);
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

bool	RequestEndDeterminator::req_is_chunked(void)
{
	if (req.request.find("\r\nTransfer-Encoding: chunked\r\n") != string::npos)
	{
		req.chunked = true;
		return (true);
	}
	return (false);
}

bool	RequestEndDeterminator::req_content_length(void)
{
	std::string	content_size;
	size_t		npos;

	size_t	pos = req.request.find("\r\nContent-Length: ");
	if (pos != std::string::npos)
	{
		npos = req.request.find("\r\n", pos + 1);
		if (npos == std::string::npos)
			return (false);
		content_size = req.request.substr(pos + strlen("\r\nContent-Length: "), npos - (pos + strlen("\r\nContent-Length: ")));
		std::stringstream sstream(content_size);
		if (!is_positive_deci(content_size) || check_overflow(content_size))
		{
			return (false);
		}
		sstream >> req.content_length;
	}
	else
		return (false);
	return (true);
}

bool	RequestEndDeterminator::check_no_body_end(void)
{
	if (req.request.find("\r\n\r\n") != std::string::npos)
		return (true);
	return (false);
}

bool	RequestEndDeterminator::check_content_end(void)
{
	if (msg_len - req.header_size >= req.content_length)
		return (true);
	return (false);
}

bool	RequestEndDeterminator::check_chunked_end(void)
{
	size_t		end_size;
	std::string	data_read;
	std::string	hexa_str;

	while (chunked_pos_in_req != std::string::npos)
	{
		if (chunked_nb_chars == -1)
		{
			end_size = req.request.find("\r\n", chunked_pos_in_req);
			if (end_size == std::string::npos)
			{
				return (false);
			}
			hexa_str = req.request.substr(chunked_pos_in_req, end_size - chunked_pos_in_req);
			if (!is_positive_hexa(hexa_str))
			{
				bad_request = true;
				return (true);
			}
			std::istringstream iss(hexa_str);
			iss >> std::hex >> chunked_nb_chars;
			chunked_pos_in_req = end_size + 2;
			chunked_data_read = "";
		}
		data_read = req.request.substr(chunked_pos_in_req);
		chunked_data_read += data_read;
		if (chunked_nb_chars == 0)
		{
			if (chunked_data_read.size() >= 2 && chunked_data_read[0] == '\r' && chunked_data_read[1] == '\n')
				return (true);
			else if (chunked_data_read.size() >= 2)
			{
				bad_request = true;
				return (true);
			}
		}
		if (chunked_data_read.size() >= (size_t) chunked_nb_chars + 2)
		{
			chunked_pos_in_req = chunked_pos_in_req + data_read.size() - chunked_data_read.size() + chunked_nb_chars + 2;
			if (req.request[chunked_pos_in_req - 2] != '\r' || req.request[chunked_pos_in_req - 1] != '\n')
			{
				bad_request = true;
				return (true);
			}
			chunked_nb_chars = -1;
		}
		else
		{
			chunked_pos_in_req += data_read.size();
			return (false);
		}
	}
	return (false);
}

void	RequestEndDeterminator::check_body(void)
{
	size_t		pos;
	std::string	header;

	pos = req.request.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		if (!req_is_chunked())
		{
			if (!req_content_length())
			{
				bad_request = true;
			}
		}
		req.header_size = pos + 4;
		chunked_pos_in_req = req.header_size;
		req.body = true;
	}
}

// attribute to mark fallthrough
// link to attributes in GCC: https://gcc.gnu.org/onlinedocs/gcc/Statement-Attributes.html

bool	RequestEndDeterminator::request_is_over(void)
{
	if (bad_request)
		return (true);

	switch (req.method)
	{
		case NONE :
			get_request_method();
			__attribute__ ((fallthrough));
		default :
			switch (req.method)
			{
				case ERROR ... DELETE : // ERROR, GET and DELETE
					return (check_no_body_end());
				case POST :
					switch (req.body)
					{
						case false :
							check_body();
							if (!req.body)
							{
								if (bad_request)
									return (true);
								return (false);
							}
							__attribute__ ((fallthrough));
						case true :
							if (req.chunked)
								return (check_chunked_end());
							else
								return (check_content_end());
					}
				case NONE :
					break ;
			}
	}
	return (false);
}
