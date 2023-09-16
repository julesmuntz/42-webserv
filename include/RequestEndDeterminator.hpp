#pragma once
#include <ctime>
#include <string>
#include <limits>

typedef enum	e_method {
	ERROR,
	GET,
	DELETE,
	POST,
	NONE
}				t_method;

typedef struct	s_time {
	bool	active;
	bool	timeout;
	time_t	start_time;
	time_t	time_passed_since;
}				t_time;

typedef struct	s_request {
	t_method	method;
	bool		chunked;
	bool		body;
	t_time		time;
	size_t		content_length;
	bool		msg_too_long;
	size_t		header_size;
	std::string	request;
}				t_request;

class RequestEndDeterminator
{
	protected:
		t_request	req;
		size_t		msg_len;
		long long	chunked_nb_chars;
		std::string	chunked_data_read;
		size_t		chunked_pos_in_req;
		bool		req_content_length(void);
		void		get_request_method(void);
		bool		req_is_chunked(void);
		bool		check_no_body_end(void);
		bool		check_content_end(void);
		bool		check_chunked_end(void);
		void		check_body(void);
		// bool		req_has_body(void);
		// bool		this_is_the_end(void);
		bool		request_is_over(void);

	public:
		RequestEndDeterminator();
		~RequestEndDeterminator();
		void	deactivate_timeout(void);
		bool	check_timeout(void);
};
