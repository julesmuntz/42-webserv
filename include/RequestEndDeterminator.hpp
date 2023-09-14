#pragma once
#include <ctime>

typedef enum	e_method {
	GET,
	DELETE,
	POST,
	NONE,
	ERROR
}				t_method;

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
	bool		body;
	t_time		time;
	bool		msg_too_long;
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
