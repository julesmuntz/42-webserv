#pragma once

#include "RequestEndDeterminator.hpp"

class RequestHandler : virtual public RequestEndDeterminator
{
	public:
		RequestHandler();
		virtual ~RequestHandler();
		bool	add_data(std::string str);
};
