#include "ResponseSender.hpp"
#include <sys/types.h>
#include <sys/socket.h>

ResponseSender::ResponseSender(int fd) : fd(fd) {}

ResponseSender::ResponseSender(int fd, string response) : fd(fd), response(response) {
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
	if (response.size() > INT_MAX)
	{
		send(fd, response.c_str(), INT_MAX, 0);
		response.erase(0, INT_MAX);
		return (false);
	}
	send(fd, response.c_str(), response.size(), 0);
	cout << "Send OK" << endl;
	return (true);
}
