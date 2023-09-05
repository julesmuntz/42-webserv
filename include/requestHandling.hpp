#pragma once

typedef enum	e_method{
	GET,
	DELETE,
	POST,
	NONE,
	ERROR
}				t_method;

typedef struct	s_request{
	t_method	method;
	bool		chunked;
	bool		body;
	//bool		error_413;
	//bool		error_431;
	std::string	request;
}		t_request;

bool	request_is_over(t_request req);