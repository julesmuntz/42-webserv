# include "RequestParser.hpp"


/**********************************************************************************/
/* ---------------------------------utils---------------------------------------- */
/**********************************************************************************/

bool recherches(const std::string& phrase, const std::string& mots)
{
	size_t pos = std::string::npos;
	pos = phrase.find(mots);

	if (pos == std::string::npos)
		return false;
	return (phrase.compare(pos, mots.length(), mots) == 0);
}

std::vector<std::string> splite(const std::string& str) {
	std::vector<std::string> result;
	std::istringstream iss(str);
	std::string token;

	while (iss >> token) {
		result.push_back(token);
	}
	return result;
}

/**********************************************************************************/
/* -------------------------constructeur destructeur----------------------------- */
/**********************************************************************************/

requestParser::requestParser(std::string request) : _request(request)
{
	std::string line;
	std::istringstream iss(this->_request);

	while (std::getline(iss, line))
		this->_lines.push_back(line);

	requestParser::set_muv();
	requestParser::set_request_header();
	requestParser::set_represent_header();
}

requestParser::~requestParser() {}

/**********************************************************************************/
/* ------------------------------geteur serveur---------------------------------- */
/**********************************************************************************/

std::string	requestParser::get_methods() const
{
	return (this->_methods);
}

std::string	requestParser::get_uri() const
{
	return (this->_uri);
}

std::string	requestParser::get_version() const
{
	return (this->_version);
}

t_Request_headers	requestParser::get_req_head() const
{
	return (this->_req_head);
}

t_General_headers	requestParser::get_gen_head() const
{
	return (this->_gen_head);
}

t_Represent_headers	requestParser::get_rep_head() const
{
	return (this->_rep_head);
}

void	requestParser::set_muv()
{
	std::vector<std::string> list = splite(this->_lines[0]);
	this->_methods = list[0];
	this->_uri = list[1];
	this->_version = list[2];
}

void	requestParser::set_request_header()
{
	std::string	lists[5] = {"Host:", "User-Agent:", "Accept:", "Accept-Language:", "Accept-Encoding:"};
	void (requestParser::*f[5])(std::string, std::string) = {&requestParser::set_host, &requestParser::set_user_agent,&requestParser::set_accept,&requestParser::set_accept_language,&requestParser::set_accept_encoding};
	long unsigned int i = 1;
	while (i < this->_lines.size())
	{
		for(int y = 0; y < 5; y++)
		{
			if (recherches(this->_lines[i], lists[y]))
				(this->*f[y])(this->_lines[i], lists[y]);
		}
		i++;
	}
}

void	requestParser::set_general_header()
{
	std::string	lists[2] = {"Connection:", "Upgrade-Insecure-Requests:"};
	void (requestParser::*f[2])(std::string, std::string) = {&requestParser::set_connection, &requestParser::set_upgrade_insecure_requests};
	long unsigned int i = 1;
	while (i < this->_lines.size())
	{
		for(int y = 0; y < 2; y++)
		{
			if (recherches(this->_lines[i], lists[y]))
				(this->*f[y])(this->_lines[i], lists[y]);
		}
		i++;
	}
}

void	requestParser::set_represent_header()
{
	std::string	lists[2] = {"Content-Type:", "Content-Length:"};
	void (requestParser::*f[2])(std::string, std::string) = {&requestParser::set_content_type, &requestParser::set_content_length};
	long unsigned int i = 1;
	while (i < this->_lines.size())
	{
		for(int y = 0; y < 2; y++)
		{
			if (recherches(this->_lines[i], lists[y]))
				(this->*f[y])(this->_lines[i], lists[y]);
		}
		i++;
	}
}

void	requestParser::set_host(std::string line, std::string mots)
{
	this->_req_head.host = line.erase(0, mots.size() + 1);
}

void	requestParser::set_user_agent(std::string line, std::string mots)
{
	this->_req_head.user_agent = line.erase(0, mots.size() + 1);
}

void	requestParser::set_accept(std::string line, std::string mots)
{
	this->_req_head.accept = line.erase(0, mots.size() + 1);
}

void	requestParser::set_accept_language(std::string line, std::string mots)
{
	this->_req_head.accept_language = line.erase(0, mots.size() + 1);
}

void	requestParser::set_accept_encoding(std::string line, std::string mots)
{
	this->_req_head.accept_encoding = line.erase(0, mots.size() + 1);
}


void	requestParser::set_connection(std::string line, std::string mots)
{
	this->_gen_head.connection = line.erase(0, mots.size() + 1);
}

void	requestParser::set_upgrade_insecure_requests(std::string line, std::string mots)
{
	this->_gen_head.upgrade_insecure_requests = line.erase(0, mots.size() + 1);
}

void	requestParser::set_content_type(std::string line, std::string mots)
{
	this->_rep_head.content_type = line.erase(0, mots.size() + 1);
}

void	requestParser::set_content_length(std::string line, std::string mots)
{
	this->_rep_head.content_length = line.erase(0, mots.size() + 1);
}
