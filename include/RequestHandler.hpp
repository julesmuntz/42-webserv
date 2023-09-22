#pragma once

#include "RequestEndDeterminator.hpp"
#include <climits>

using namespace std;

#define HEADER_MAX_SIZE	10000
#define MSG_MAX_SIZE	INT_MAX

typedef enum	e_error {
	error_400 = 400, // bad request
	error_404 = 404, // not found
	error_405 = 405, // method not allowed
	error_408 = 408, // timeout
	error_413 = 413, // request header too long
	error_431 = 431, // request body too long
	no_error = 0
}				t_error;

class RequestHandler : virtual public RequestEndDeterminator
{
	private:
		int		fd;
		t_error	error;

	public:
		RequestHandler(int fd);
		virtual ~RequestHandler();
		string	get_request_string(void) const;
		t_error	get_error(void) const;
		bool	add_data(char *str, size_t nread);
		void	check_preparsing_errors(void); //timeout, request header too long, and msg too long (request body too long), and bad request
};
