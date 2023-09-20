#pragma once

#include "RequestEndDeterminator.hpp"

class ResponseSender
{
	private:
		int		fd;
		int		whereweare;
		string	response;

	public:
		ResponseSender(int fd);
		ResponseSender(int fd, string response);
		~ResponseSender();
		void	setResponse(string &resp);
		bool	send_response(void);
};
