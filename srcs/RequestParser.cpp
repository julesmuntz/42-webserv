# include "RequestParser.hpp"


/**********************************************************************************/
/* ---------------------------------utils---------------------------------------- */
/**********************************************************************************/

bool recherches(const string& phrase, const string& mots)
{
	size_t pos = string::npos;
	pos = phrase.find(mots);

	if (pos == string::npos)
		return false;
	return (phrase.compare(pos, mots.length(), mots) == 0);
}

vector<string> splite(const string& str) {
	vector<string> result;
	istringstream iss(str);
	string token;

	while (iss >> token) {
		result.push_back(token);
	}
	return result;
}

/**********************************************************************************/
/* -------------------------constructeur destructeur----------------------------- */
/**********************************************************************************/

RequestParser::RequestParser() {}

RequestParser::RequestParser(string request) : _request(request)
{
	string line;
	istringstream iss(this->_request);

	while (getline(iss, line))
		this->_lines.push_back(line);

	RequestParser::set_muv();
	RequestParser::set_request_header();
	RequestParser::set_represent_header();
}

RequestParser	&RequestParser::operator=(const RequestParser &ref)
{
	this->_methods = ref._methods;
	this->_uri = ref._uri;
	this->_version = ref._version;
	this->_gen_head = ref._gen_head;
	this->_req_head = ref._req_head;
	this->_rep_head = ref._rep_head;

	return (*this);
}

RequestParser::~RequestParser() {}

/**********************************************************************************/
/* ------------------------------geteur serveur---------------------------------- */
/**********************************************************************************/

string	RequestParser::get_methods() const
{
	return (this->_methods);
}

string	RequestParser::get_uri() const
{
	return (this->_uri);
}

string	RequestParser::get_version() const
{
	return (this->_version);
}

t_Request_headers	RequestParser::get_req_head() const
{
	return (this->_req_head);
}

t_General_headers	RequestParser::get_gen_head() const
{
	return (this->_gen_head);
}

t_Represent_headers	RequestParser::get_rep_head() const
{
	return (this->_rep_head);
}

void	RequestParser::set_muv()
{
	vector<string> list = splite(this->_lines[0]);
	this->_methods = list[0];
	this->_uri = list[1];
	this->_version = list[2];
}

void	RequestParser::set_request_header()
{
	string	lists[5] = {"Host:", "User-Agent:", "Accept:", "Accept-Language:", "Accept-Encoding:"};
	void (RequestParser::*f[5])(string, string) = {&RequestParser::set_host, &RequestParser::set_user_agent,&RequestParser::set_accept,&RequestParser::set_accept_language,&RequestParser::set_accept_encoding};
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

void	RequestParser::set_general_header()
{
	string	lists[2] = {"Connection:", "Upgrade-Insecure-Requests:"};
	void (RequestParser::*f[2])(string, string) = {&RequestParser::set_connection, &RequestParser::set_upgrade_insecure_requests};
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

void	RequestParser::set_represent_header()
{
	string	lists[2] = {"Content-Type:", "Content-Length:"};
	void (RequestParser::*f[2])(string, string) = {&RequestParser::set_content_type, &RequestParser::set_content_length};
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

void	RequestParser::set_host(string line, string mots)
{

	line = line.erase(0, mots.size() + 1);
	if (line.find(":") != string::npos)
	{
		this->_req_head.hosts = pair<string, uint32_t>(line.erase(0, line.find(":")), atoi(line.erase(1, line.size()).c_str()));
	}
}

void	RequestParser::set_user_agent(string line, string mots)
{
	this->_req_head.user_agent = line.erase(0, mots.size() + 1);
}

void	RequestParser::set_accept(string line, string mots)
{
	this->_req_head.accept = line.erase(0, mots.size() + 1);
}

void	RequestParser::set_accept_language(string line, string mots)
{
	this->_req_head.accept_language = line.erase(0, mots.size() + 1);
}

void	RequestParser::set_accept_encoding(string line, string mots)
{
	this->_req_head.accept_encoding = line.erase(0, mots.size() + 1);
}


void	RequestParser::set_connection(string line, string mots)
{
	this->_gen_head.connection = line.erase(0, mots.size() + 1);
}

void	RequestParser::set_upgrade_insecure_requests(string line, string mots)
{
	this->_gen_head.upgrade_insecure_requests = line.erase(0, mots.size() + 1);
}

void	RequestParser::set_content_type(string line, string mots)
{
	this->_rep_head.content_type = line.erase(0, mots.size() + 1);
}

void	RequestParser::set_content_length(string line, string mots)
{
	this->_rep_head.content_length = line.erase(0, mots.size() + 1);
}
