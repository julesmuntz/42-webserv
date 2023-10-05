#pragma once

#include "RequestEndDeterminator.hpp"

class ResponseSender
{
	private:
		int		fd;
		size_t	whereweare;
		string	response;
		string	_header;
		bool	_transfert;
		
		void	send_chunked(size_t size);
	public:
		ResponseSender(int fd);
		ResponseSender(int fd, string response, bool transfert);
		~ResponseSender();
		void	setResponse(string &resp);
		bool	send_response(void);
};
