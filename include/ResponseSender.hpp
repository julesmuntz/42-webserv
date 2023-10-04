#pragma once

#include "RequestEndDeterminator.hpp"

class ResponseSender
{
	private:
		int		fd;
		size_t	whereweare;
		string	response;
		bool	_transfert;
	public:
		ResponseSender(int fd);
		ResponseSender(int fd, string response, bool transfert);
		~ResponseSender();
		void	setResponse(string &resp);
		bool	send_response(void);
};
