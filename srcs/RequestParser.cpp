# include "RequestParser.hpp"

/**********************************************************************************/
/* -------------------------constructeur destructeur----------------------------- */
/**********************************************************************************/

RequestParser::RequestParser() {}

RequestParser::RequestParser(string request) : _request(request)
{
	string	line;
	size_t	pos;
	string	header;
	//determine header

	pos = this->_request.find("\r\n\r\n");
	if (pos != string::npos)
		header = _request.substr(0, pos);
	else
		header = _request;

	istringstream iss(header);

	while (getline(iss, line))
		this->_lines.push_back(line);

	// pay attention to only check in header for header stuff
	if (!this->_lines.empty())
	{
		RequestParser::set_muv();
		RequestParser::set_request_header();
		RequestParser::set_represent_header();
	}
	RequestParser::set_body();
}

RequestParser	&RequestParser::operator=(const RequestParser &ref)
{
	this->_method = ref._method;
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

string	RequestParser::get_method() const
{
	return (this->_method);
}

string	RequestParser::get_uri() const
{
	return (this->_uri);
}

string	RequestParser::get_version() const
{
	return (this->_version);
}

string	RequestParser::get_body() const
{
	return (this->_body);
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
	vector<string> list = split(this->_lines[0]);
	this->_method = list[0];
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
			if (recherche(this->_lines[i], lists[y]))
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
			if (recherche(this->_lines[i], lists[y]))
				(this->*f[y])(this->_lines[i], lists[y]);
		}
		i++;
	}
}

void	RequestParser::set_represent_header()
{
	string	lists[3] = {"Content-Type:", "Content-Length:", "Transfer-Encoding:"};
	void (RequestParser::*f[3])(string, string) = {&RequestParser::set_content_type, &RequestParser::set_content_length, &RequestParser::set_transfer_encoding};
	long unsigned int i = 1;
	while (i < this->_lines.size())
	{
		for(int y = 0; y < 3; y++)
		{
			if (recherche(this->_lines[i], lists[y]))
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
		pair<string, uint32_t> tests;
		tests.first = line.substr(0, line.find(":"));
		tests.second = atoi(line.substr(line.find(":") + 1, line.size()).c_str());
		this->_req_head.hosts = tests;
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

void	RequestParser::set_transfer_encoding(string line, string mots)
{
	this->_rep_head.transfer_encoding = line.erase(0, mots.size() + 1);
}

void	RequestParser::set_body()
{
	size_t	pos;

	this->_body = "";

	pos = this->_request.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		this->_body = this->_request.substr(pos + 4);
		if (_rep_head.transfer_encoding == "chunked")
		{
			this->dechunk_body();
		}
	}
}

void	RequestParser::dechunk_body()
{
	string	unchunked_body = "";
	string	hexa_str;
	size_t	hexa;
	string	data_str;
	string	body = this->_body;

	while (true)
	{
		size_t end_pos = body.find("\r\n");
		if (end_pos != string::npos)
		{
			hexa_str = body.substr(0, end_pos);
			body.erase(0, end_pos + 2);
			istringstream iss(hexa_str);
			iss >> hex >> hexa;
			data_str = body.substr(0, hexa);
			unchunked_body += data_str;
			if (hexa + 2 != string::npos)
				body.erase(0, hexa + 2);
		}
		else
			break;
	}
	this->_body = unchunked_body;
}
