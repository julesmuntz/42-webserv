#include "ResponseSender.hpp"
#include <sys/types.h>
#include <sys/socket.h>

ResponseSender::ResponseSender(int fd) : fd(fd) {}

ResponseSender::ResponseSender(int fd, string response, bool transfert) : fd(fd), response(response), _transfert(transfert) {
	if (response.size() < INT_MAX)
		_transfert = false;
}

ResponseSender::~ResponseSender() {}

void	ResponseSender::setResponse(string &resp)
{
	response = resp;
}

bool	ResponseSender::send_response(void)
{
	// check return value of send
	cout << "Sending..." << endl;
	if (response.size() > INT_MAX)//verifier si l'envoye par chunck est posible
	{
		send(fd, response.c_str(), INT_MAX, 0);
		response.erase(0, INT_MAX);
		return (false);
	}
	// if (_transfert && response.size() > INT_MAX || _transfert)
	if(false)
	{
		//changer le header en premier
		size_t	pos_begin = response.find("Content-Length: ");
		size_t	pos_end = string::npos;
		if (pos_begin != string::npos)
			pos_end = response.find("\r\n", pos_begin + 1);
		if (pos_begin != string::npos && pos_end != string::npos)
			response.replace(pos_begin,  (pos_end - pos_begin), "Content-Length: chunked");

		if (response.size() > INT_MAX)
		{
			std::stringstream stream;
			stream << std::hex << INT_MAX;
			std::string result( stream.str() );

			response.insert(INT_MAX - 2, "\r\n");
			send(fd, result.c_str(), result.size(), 0);
			send(fd, response.c_str(), INT_MAX, 0);
			response.erase(0, INT_MAX);
		}
		else if (response.empty())
		{
			std::stringstream stream;
			stream << std::hex << response.size() + 2;
			std::string result( stream.str() );
			response.insert(response.size(), "\r\n");
			send(fd, result.c_str(), result.size(), 0);
			send(fd, response.c_str(), response.size(), 0);
			response.erase(0, response.size());
		}

		if (!response.empty())
			return (send(fd, "0", 1, 0), true);
		return (false);
	}
	send(fd, response.c_str(), response.size(), 0);
	cout << "Send OK" << endl;
	return (true);
}
