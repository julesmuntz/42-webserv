#pragma once

typedef enum	e_error {
	error_400 = 400, // bad request
	error_404 = 404, // not found
	error_408 = 408, // timeout
	error_413 = 413, // request header too long
	error_431 = 431,  // request body too long
	no_error = 0
}				t_error;

#include "RequestEndDeterminator.hpp"

class RequestHandler : virtual public RequestEndDeterminator
{
	private:
		int		fd;
		size_t	msg_len;
		t_error	error;

	public:
		RequestHandler(int fd);
		virtual ~RequestHandler();
		bool	add_data(std::string str);
		bool	check_preparsing_errors(void); //timeout, request header too long, and msg too long (request body too long)
};
