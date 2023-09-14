#pragma one

#include "RequestParser.hpp"
#include <iostream>
#include <string>

typedef struct s_responce
{
	std::string	status;
	std::string	header;
	std::string	body;
}		t_responce;


class ResponseHTTP
{
	private:
		requestParser _request;
		t_responce	_responce;
	public:
		ResponseHTTP(const requestParser &);
		~ResponseHTTP();

		void	setup()
		{
			_responce.status = _request.get_version() + " 200 OK";
		}
		t_responce	get_responce() const
		{
			return _responce;
		}
};
