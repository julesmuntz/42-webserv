#include "ResponseHTTP.hpp"

static std::map<uint32_t, std::string> generate_static_code()
{
	std::map<uint32_t, std::string> result;

	result.insert(std::pair<uint32_t, std::string>(100, "Continue"));
	result.insert(std::pair<uint32_t, std::string>(101, "Switching Protocol"));
	result.insert(std::pair<uint32_t, std::string>(102, "Processing"));
	result.insert(std::pair<uint32_t, std::string>(103, "Early Hints"));
	result.insert(std::pair<uint32_t, std::string>(200, "OK"));
	result.insert(std::pair<uint32_t, std::string>(201, "Created"));
	result.insert(std::pair<uint32_t, std::string>(202, "Accepted"));
	result.insert(std::pair<uint32_t, std::string>(203, "Non-Authoritative Information"));
	result.insert(std::pair<uint32_t, std::string>(204, "No Content"));
	result.insert(std::pair<uint32_t, std::string>(205, "Reset Content"));
	result.insert(std::pair<uint32_t, std::string>(206, "Partial Content"));
	result.insert(std::pair<uint32_t, std::string>(207, "Multi-Status"));
	result.insert(std::pair<uint32_t, std::string>(208, "Already Reported"));
	result.insert(std::pair<uint32_t, std::string>(226, "IM Used"));
	result.insert(std::pair<uint32_t, std::string>(300, "Multiple Choices"));
	result.insert(std::pair<uint32_t, std::string>(301, "Moved Permanently"));
	result.insert(std::pair<uint32_t, std::string>(302, "Found"));
	result.insert(std::pair<uint32_t, std::string>(303, "see Other"));
	result.insert(std::pair<uint32_t, std::string>(304, "Not Modified"));
	result.insert(std::pair<uint32_t, std::string>(307, "Temporary Redirect"));
	result.insert(std::pair<uint32_t, std::string>(308, "Permanent Redirect"));
	result.insert(std::pair<uint32_t, std::string>(400, "Bad Request"));
	result.insert(std::pair<uint32_t, std::string>(401, "Unauthorized"));
	result.insert(std::pair<uint32_t, std::string>(402, "Payment Required"));
	result.insert(std::pair<uint32_t, std::string>(403, "Forbidden"));
	result.insert(std::pair<uint32_t, std::string>(404, "Not Found"));
	result.insert(std::pair<uint32_t, std::string>(405, "Method Not Allowed"));
	result.insert(std::pair<uint32_t, std::string>(406, "Not Acceptable"));
	result.insert(std::pair<uint32_t, std::string>(407, "Proxy Authentication Required"));
	result.insert(std::pair<uint32_t, std::string>(408, "Request Timeout"));
	result.insert(std::pair<uint32_t, std::string>(409, "Conflict"));
	result.insert(std::pair<uint32_t, std::string>(410, "Gone"));
	result.insert(std::pair<uint32_t, std::string>(411, "Length Required"));
	result.insert(std::pair<uint32_t, std::string>(412, "Precondition Failed"));
	result.insert(std::pair<uint32_t, std::string>(413, "Payload Too Large"));
	result.insert(std::pair<uint32_t, std::string>(414, "URI Too Long"));
	result.insert(std::pair<uint32_t, std::string>(415, "Unsupported Media Type"));
	result.insert(std::pair<uint32_t, std::string>(416, "Range Not Satisfiable"));
	result.insert(std::pair<uint32_t, std::string>(417, "Expectation Failed"));
	result.insert(std::pair<uint32_t, std::string>(418, "I'm a teapot"));
	result.insert(std::pair<uint32_t, std::string>(421, "Misdirected Request"));
	result.insert(std::pair<uint32_t, std::string>(422, "Unprocessable Entity"));
	result.insert(std::pair<uint32_t, std::string>(423, "Locked"));
	result.insert(std::pair<uint32_t, std::string>(424, "Failed Dependency"));
	result.insert(std::pair<uint32_t, std::string>(425, "Too Early"));
	result.insert(std::pair<uint32_t, std::string>(426, "Upgrade Required"));
	result.insert(std::pair<uint32_t, std::string>(428, "Precondition Required"));
	result.insert(std::pair<uint32_t, std::string>(429, "Too Many Requests"));
	result.insert(std::pair<uint32_t, std::string>(431, "Request Header Fields Too Large"));
	result.insert(std::pair<uint32_t, std::string>(451, "Unavailable For Legal Reasons"));
	result.insert(std::pair<uint32_t, std::string>(500, "Internal Server Error"));
	result.insert(std::pair<uint32_t, std::string>(501, "Not Implemented"));
	result.insert(std::pair<uint32_t, std::string>(502, "Bad Gateway"));
	result.insert(std::pair<uint32_t, std::string>(503, "Service Unavailable"));
	result.insert(std::pair<uint32_t, std::string>(504, "Gateway Timeout"));
	result.insert(std::pair<uint32_t, std::string>(505, "HTTP Version Not Supported"));
	result.insert(std::pair<uint32_t, std::string>(506, "Variant Also Negotiates"));
	result.insert(std::pair<uint32_t, std::string>(507, "Insufficient Storage"));
	result.insert(std::pair<uint32_t, std::string>(508, "Loop Detected"));
	result.insert(std::pair<uint32_t, std::string>(510, "Not Extended"));
	result.insert(std::pair<uint32_t, std::string>(511, "Network Authentication Required"));

	return (result);
}

ResponseHTTP::ResponseHTTP(RequestParser &request, std::vector<t_server> &server_config)
{
	this->_static_code = generate_static_code();
	this->_request = request;
	this->_server_config = server_config;
	this->_location = false;
}

ResponseHTTP::~ResponseHTTP(){}