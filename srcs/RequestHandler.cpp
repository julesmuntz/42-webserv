#include "Server.hpp"

RequestHandler::RequestHandler(int fd) : RequestEndDeterminator(), fd(fd), error(no_error) {}

RequestHandler::~RequestHandler() {}

bool	RequestHandler::check_preparsing_errors(void)
{
	if (req.time.timeout == true) // check timeout
		error = error_408;
	else if (req.msg_too_long) // check message size
		error = error_413;
	else // check header size
	{
		std::string	msg;
		std::string	header;
		msg = req.request;
		size_t	pos = msg.find_first_of("\r\n\r\n");
		header = msg.substr(0, pos);
		if (header.size() + 2 > HEADER_MAX_SIZE)
			error = error_431;
	}
	if (error)
		return (true);
	return (false);
}

bool	RequestHandler::add_data(char *str, size_t nread)
{
	std::string	addon;

	addon.assign(str, nread);
	req.request += addon;
	msg_len += nread;
	if (msg_len > MSG_MAX_SIZE)
	{
		req.msg_too_long = true;
		return (false);
	}
	std::cout << "Request is: " << req.request << std::endl;
	return (!this->request_is_over());
}
