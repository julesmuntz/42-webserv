#pragma once

#include "RequestEndDeterminator.hpp"

class RequestHandler : virtual public RequestEndDeterminator
{
	private:
		int	fd;

	public:
		RequestHandler(int fd);
		virtual ~RequestHandler();
		bool	add_data(std::string str);
};
