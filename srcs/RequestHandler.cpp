#include "Server.hpp"

RequestHandler::RequestHandler() : RequestEndDeterminator() {}

RequestHandler::~RequestHandler() {}

bool	RequestHandler::add_data(std::string str)
{
	req.request += str;
	std::cout << "Request is: " << req.request << std::endl;
	return (!this->request_is_over());
}
