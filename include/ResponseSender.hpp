#pragma once

#include "RequestEndDeterminator.hpp"

class ResponseSender
{
	private:
		int		fd;
		size_t	whereweare;
		string	response;

	public:
		ResponseSender(int fd);
		ResponseSender(int fd, string response);
		~ResponseSender();
		void	setResponse(string &resp);
		bool	send_response(void);
};
