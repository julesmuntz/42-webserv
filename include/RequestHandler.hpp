#pragma once

typedef enum	e_method{
	GET,
	DELETE,
	POST,
	NONE,
	ERROR
}				t_method;

typedef enum	e_error{
	error_400 = 400,
	error_404 = 404,
	error_413 = 413,
	error_431 = 431
}				t_error;

typedef struct	s_request{
	t_method	method;
	bool		chunked;
	bool		body;
	std::string	request;
}				t_request;

class RequestHandler
{
	private:
		t_request	req;
		bool		request_is_over(void);

	public:
		RequestHandler();
		~RequestHandler();
		bool	add_data(std::string str);
};
