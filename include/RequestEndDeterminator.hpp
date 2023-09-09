#pragma once
#include <ctime>

typedef enum	e_method {
	GET,
	DELETE,
	POST,
	NONE,
	ERROR
}				t_method;

typedef enum	e_error {
	error_400 = 400, // bad request
	error_404 = 404, // not found
	error_408 = 408, // timeout
	error_413 = 413, // request header too long
	error_431 = 431  // request body too long
}				t_error;

typedef struct	s_time {
	bool	active;
	bool	timeout;
	time_t	start_time;
	time_t	time_passed_since;
}				t_time;

//I guess we can check the size with stat, we will approximate body size = file size
//Because we won't allow header size to be too big
//Or else we would need a differential operation (I mean a substraction)

typedef struct	s_request {
	t_method	method;
	bool		chunked;
	size_t		header_size;
	size_t		body_size;
	bool		body;
	t_time		time;
	std::string	request;
}				t_request;

class RequestEndDeterminator
{
	protected:
		t_request	req;
		void		get_request_method(void);
		bool		req_is_chunked(void);
		bool		req_has_body(void);
		bool		this_is_the_end(void);
		bool		request_is_over(void);

	public:
		RequestEndDeterminator();
		~RequestEndDeterminator();
		void	deactivate_timeout(void);
		bool	check_timeout(void);
};
