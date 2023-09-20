#include "ResponseHTTP.hpp"

/**********************************************************************************/
/* -------------------------constructeur destructeur----------------------------- */
/**********************************************************************************/

static map<uint32_t, string> generate_static_code()
{
	map<uint32_t, string> result;

	result.insert(pair<uint32_t, string>(100, "Continue"));
	result.insert(pair<uint32_t, string>(101, "Switching Protocol"));
	result.insert(pair<uint32_t, string>(102, "Processing"));
	result.insert(pair<uint32_t, string>(103, "Early Hints"));
	result.insert(pair<uint32_t, string>(200, "OK"));
	result.insert(pair<uint32_t, string>(201, "Created"));
	result.insert(pair<uint32_t, string>(202, "Accepted"));
	result.insert(pair<uint32_t, string>(203, "Non-Authoritative Information"));
	result.insert(pair<uint32_t, string>(204, "No Content"));
	result.insert(pair<uint32_t, string>(205, "Reset Content"));
	result.insert(pair<uint32_t, string>(206, "Partial Content"));
	result.insert(pair<uint32_t, string>(207, "Multi-Status"));
	result.insert(pair<uint32_t, string>(208, "Already Reported"));
	result.insert(pair<uint32_t, string>(226, "IM Used"));
	result.insert(pair<uint32_t, string>(300, "Multiple Choices"));
	result.insert(pair<uint32_t, string>(301, "Moved Permanently"));
	result.insert(pair<uint32_t, string>(302, "Found"));
	result.insert(pair<uint32_t, string>(303, "see Other"));
	result.insert(pair<uint32_t, string>(304, "Not Modified"));
	result.insert(pair<uint32_t, string>(307, "Temporary Redirect"));
	result.insert(pair<uint32_t, string>(308, "Permanent Redirect"));
	result.insert(pair<uint32_t, string>(400, "Bad Request"));
	result.insert(pair<uint32_t, string>(401, "Unauthorized"));
	result.insert(pair<uint32_t, string>(402, "Payment Required"));
	result.insert(pair<uint32_t, string>(403, "Forbidden"));
	result.insert(pair<uint32_t, string>(404, "Not Found"));
	result.insert(pair<uint32_t, string>(405, "Method Not Allowed"));
	result.insert(pair<uint32_t, string>(406, "Not Acceptable"));
	result.insert(pair<uint32_t, string>(407, "Proxy Authentication Required"));
	result.insert(pair<uint32_t, string>(408, "Request Timeout"));
	result.insert(pair<uint32_t, string>(409, "Conflict"));
	result.insert(pair<uint32_t, string>(410, "Gone"));
	result.insert(pair<uint32_t, string>(411, "Length Required"));
	result.insert(pair<uint32_t, string>(412, "Precondition Failed"));
	result.insert(pair<uint32_t, string>(413, "Payload Too Large"));
	result.insert(pair<uint32_t, string>(414, "URI Too Long"));
	result.insert(pair<uint32_t, string>(415, "Unsupported Media Type"));
	result.insert(pair<uint32_t, string>(416, "Range Not Satisfiable"));
	result.insert(pair<uint32_t, string>(417, "Expectation Failed"));
	result.insert(pair<uint32_t, string>(418, "I'm a teapot"));
	result.insert(pair<uint32_t, string>(421, "Misdirected Request"));
	result.insert(pair<uint32_t, string>(422, "Unprocessable Entity"));
	result.insert(pair<uint32_t, string>(423, "Locked"));
	result.insert(pair<uint32_t, string>(424, "Failed Dependency"));
	result.insert(pair<uint32_t, string>(425, "Too Early"));
	result.insert(pair<uint32_t, string>(426, "Upgrade Required"));
	result.insert(pair<uint32_t, string>(428, "Precondition Required"));
	result.insert(pair<uint32_t, string>(429, "Too Many Requests"));
	result.insert(pair<uint32_t, string>(431, "Request Header Fields Too Large"));
	result.insert(pair<uint32_t, string>(451, "Unavailable For Legal Reasons"));
	result.insert(pair<uint32_t, string>(500, "Internal Server Error"));
	result.insert(pair<uint32_t, string>(501, "Not Implemented"));
	result.insert(pair<uint32_t, string>(502, "Bad Gateway"));
	result.insert(pair<uint32_t, string>(503, "Service Unavailable"));
	result.insert(pair<uint32_t, string>(504, "Gateway Timeout"));
	result.insert(pair<uint32_t, string>(505, "HTTP Version Not Supported"));
	result.insert(pair<uint32_t, string>(506, "Variant Also Negotiates"));
	result.insert(pair<uint32_t, string>(507, "Insufficient Storage"));
	result.insert(pair<uint32_t, string>(508, "Loop Detected"));
	result.insert(pair<uint32_t, string>(510, "Not Extended"));
	result.insert(pair<uint32_t, string>(511, "Network Authentication Required"));

	return (result);
}

// ResponseHTTP::ResponseHTTP(t_server, t_server server_config)
// {
// 	this->_static_code = generate_static_code();
// }

ResponseHTTP::ResponseHTTP(RequestParser &request, t_server server_config)
{
	this->_static_code = generate_static_code();
	this->_request = request;
	this->_server_config = server_config;
	this->_no_location = this->set_location();
	this->_response_string = DUMMY_RESPONSE;
}

ResponseHTTP::~ResponseHTTP(){}

bool	ResponseHTTP::set_location()
{

	if (!_request.get_uri().empty())
		return (true);
	for (vector<t_location>::iterator it = _server_config.location.begin(); it != _server_config.location.end(); it++)
	{
		if (it->file_location == _request.get_uri())
			return (false);
	}
	return (true);
}

/**********************************************************************************/
/* ---------------------------genertate response--------------------------------- */
/**********************************************************************************/

void	ResponseHTTP::generate_400_error(int code)
{
	map<uint32_t, string>::iterator it = _static_code.find(code);

	if (it != _static_code.end())
	{
		_header = ERRORHEAD;
		//_body = ERRORBODY((it->first), (it->second));
		_body = ERRORBODY_PART_1;
		_body += it->first;
		_body += ERRORBODY_PART_2;
		_body += it->second;
		_body += ERRORBODY_PART_3;
		_html = _header + _body;
		_response << "HTTP/1.1 " << it->first << " " << it->second << "\r\n";
		_response << "Content-Type: text/html\r\n";
		_response << "Content-Length: " << _html.length() << "\r\n";
		_response << "\r\n";
	}
}

/**********************************************************************************/
/* ----------------------------------setup--------------------------------------- */
/**********************************************************************************/

void	ResponseHTTP::setup()
{
	if (_no_location)
		return (generate_400_error(404));
	//faire un if pour too long .. etc
	if (_request.get_methods() == "GET")
		return ;
}


string	ResponseHTTP::get_response_string(void) const
{
	return (_response_string);
}
