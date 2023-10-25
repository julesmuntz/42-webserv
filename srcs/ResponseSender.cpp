#include "ResponseSender.hpp"
#include <sys/types.h>
#include <sys/socket.h>

ResponseSender::ResponseSender(int fd) : fd(fd) {}

ResponseSender::ResponseSender(int fd, string respons, bool transfert) : fd(fd), response(respons), _transfert(transfert) {
	if (response.size() < INT_MAX)
		_transfert = false;
	if (_transfert || response.size() > INT_MAX)
	{
		size_t	pos_begin = response.find("Content-Length: ");
		if (pos_begin != string::npos)
		{
			size_t pos_end = response.find("\r\n", pos_begin + 1);
			if (pos_begin != string::npos && pos_end != string::npos)
				response.replace(pos_begin,  (pos_end - pos_begin), "Transfer-Encoding: chunked");
		}
		size_t pos_end_header = response.find("\r\n\r\n");
		if (pos_end_header != string::npos)
		{
			_header = response.substr(0, pos_end_header + 4);
			response.erase(0, pos_end_header + 4);
		}
		_transfert = true;
	}
}

ResponseSender::~ResponseSender() {}

void	ResponseSender::setResponse(string &resp)
{
	response = resp;
}

void	ResponseSender::send_chunked(size_t size)
{
	std::stringstream stream;
	stream << std::hex << response.size();
	std::string result( stream.str() );

	send(fd, result.c_str(), result.size(), 0);
	send(fd, "\r\n", 2, 0);
	send(fd, response.c_str(), size, 0);
	send(fd, "\r\n", 2, 0);

	response.erase(0, size);
}

bool	ResponseSender::send_response(void)
{
	cout << "Sending..." << endl;

	if (_transfert)
	{
		if (!_header.empty())
		{
			send(fd, _header.c_str(), _header.size(), 0);
			_header.erase(0, _header.size());
		}
		if (response.size() > INT_MAX)
			send_chunked(INT_MAX);
		else if (response.size() < INT_MAX)
			send_chunked(response.size());
		if (response.empty())
			return (send(fd, "0\r\n\r\n", 5, 0), true);
		return (false);
	}
	send(fd, response.c_str(), response.size(), 0);
	cout << "Send OK" << endl;
	return (true);
}


