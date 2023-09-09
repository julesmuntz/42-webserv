#include "Server.hpp"
#include <sys/stat.h>

RequestHandler::RequestHandler(int fd) : RequestEndDeterminator(), fd(fd) {}

RequestHandler::~RequestHandler() {}

//It will be number in bytes not in characters, so unicode will be greater, or should be
// so we can just calculate the size normally with string::size or string::length
// and they return the number of bytes

bool	RequestHandler::add_data(std::string str)
{
	req.request += str;
	std::cout << "Request is: " << req.request << std::endl;
	return (!this->request_is_over());
}
