#include "Exodus.hpp"

/**********************************************************************************/
/* ---------------------------------utils---------------------------------------- */
/**********************************************************************************/

bool recherche(const string& phrase, const string& mots)
{
	size_t pos = string::npos;
	pos = phrase.find(mots);

	if (pos == string::npos)
		return false;
	return (phrase.compare(pos, mots.length(), mots) == 0);
}

vector<string> split(const string& str) {
	vector<string> result;
	istringstream iss(str);
	string token;

	while (iss >> token) {
		result.push_back(token);
	}
	return result;
}

string extractmots(string line, string mots) {
	vector<string> tokens = split(line);

	vector<string>::iterator it = find(tokens.begin(), tokens.end(), mots);
	if (it == tokens.end())
		return ("");
	size_t listenIndex = distance(tokens.begin(), it) + 1;
	if (listenIndex >= tokens.size())
		return ("");
	return tokens[listenIndex];
}

/**********************************************************************************/
/* -------------------------constructeur destructeur----------------------------- */
/**********************************************************************************/

Exodus::Exodus(string const filename)
{
	string		extantion;

	extantion = FILE_CONF;
	if (filename.size() >= extantion.size() && filename.compare(filename.size() - extantion.size(), extantion.size(), extantion))
		throw Exodus::error_filename();
	this->_ifs.open(filename.c_str(), ifstream::in);
	if (!this->_ifs.is_open())
		throw Exodus::error_open();
}

Exodus::~Exodus()
{
	this->_ifs.close();
}

/**********************************************************************************/
/* ------------------------------geteur serveur---------------------------------- */
/**********************************************************************************/

vector<t_server>	Exodus::get_server() const
{
	return (this->_server);
}

/**********************************************************************************/
/* ----------------------------------setup--------------------------------------- */
/**********************************************************************************/

void	Exodus::setup()
{
	string line;

	while (getline(this->_ifs, line))
	{
		if (recherche(line, "server") && recherche(line, "{"))
			this->_server.push_back(set_server());
	}
	for(long unsigned int i = 0; i < this->_server.size(); i++)
	{
		if (this->_server[i].listen.first == 0 || this->_server[i].listen.second.empty() || this->_server[i].server_name.empty() || this->_server[i].client_body_size == 0)
		{
			cout << "config de defautl" << endl;
			Exodus def(FILE_DEFAULT);
			this->_server = def._server;
			return ;
		}
		for(long unsigned int y = 0; y < this->_server[i].location.size(); y++)
		{
			if (this->_server[i].location[y].redir_link.empty())
				if (this->_server[i].location[y].uri.empty() || this->_server[i].location[y].allow_methods.empty() || this->_server[i].location[y].root.empty() || this->_server[i].location[y].index.empty() || this->_server[i].location[y].file_location.empty())
				{
					cout << "config de defautl" << endl;
					Exodus def(FILE_DEFAULT);
					this->_server = def._server;
					return ;
				}
		}
	}
}

/**********************************************************************************/
/* -------------------------seteur server location------------------------------- */
/**********************************************************************************/

t_server	Exodus::set_server()
{
	string	line;
	t_server	exodus_server;
	string servers[4] = {"listen" ,"server_name" ,"client_body_size","error_page"};
	void		(Exodus::*f[4])(t_server *, string) = {&Exodus::listen, &Exodus::server_name, &Exodus::client_body_size, &Exodus::error_page};
	bool		end = false;

	while (!end && getline(this->_ifs, line))
	{
		if (recherche(line, "location") && recherche(line, "{"))
			exodus_server.location.push_back(set_location(line));
		else if (recherche(line, "}"))
			end = true;
		for(long unsigned int i = 0; i < 4; i++)
		{
			if (recherche(line, servers[i]))
			{
				(this->*f[i])(&exodus_server, line);
				break ;
			}
		}

	}
	if (!end)
		throw Exodus::error_end();
	return (exodus_server);
}

t_location				Exodus::set_location(string line)
{
	bool		end = false;
	t_location	exodus_location;
	string	locations[8] = {"uri", "allow_methods", "root", "redir_link", "index", "directory_listing", "file_location", "cgi_pass"};
	void		(Exodus::*f[8])(t_location *, string) = {&Exodus::uri, &Exodus::allow_methods, &Exodus::root, &Exodus::redir_link, &Exodus::index, &Exodus::directory_listing, &Exodus::file_location, &Exodus::cgi_pass};

	exodus_location.directory_listing = false;

	uri(&exodus_location, line);
	while (!end && getline(this->_ifs, line) )
	{
		for(long unsigned int i = 0; i < 8; i++)
		{

			if (recherche(line, locations[i]))
			{
				(this->*f[i])(&exodus_location, line);
				break ;
			}
		}
		if (recherche(line, "}"))
			end = true;
	}
	if (!end)
		throw Exodus::error_end();
	return (exodus_location);
}

/**********************************************************************************/
/* -------------------------------server----------------------------------------- */
/**********************************************************************************/

void					Exodus::listen(t_server *t, string line)
{
	string extractedWord = extractmots(line, "listen");
	if (!extractedWord.empty())
	{
		if (!recherche(extractedWord, ":"))
			throw logic_error("listen");
		size_t delimiterPos = extractedWord.find(":");
		if (delimiterPos != string::npos) {
			t->listen.second = extractedWord.substr(0, delimiterPos);
			t->listen.first = strtol(extractedWord.substr(delimiterPos + 1).c_str(), NULL, 10);
		}
		else
			throw logic_error("listen");
	}
	else
		throw logic_error("listen");
}

void					Exodus::server_name(t_server *t, string line)
{
	if (recherche(line, "server_name"))
	{
		vector<string> tokens = split(line);
		vector<string>::iterator it = find(tokens.begin(), tokens.end(), "server_name");
		if (it == tokens.end())
			throw logic_error("server name");
		size_t listenIndex = distance(tokens.begin(), it) + 1;
		if (listenIndex >= tokens.size())
			throw logic_error("server name");
		while (listenIndex < tokens.size())
			t->server_name.push_back(tokens[listenIndex++]);
	}
	else
		throw logic_error("server name");
}

void					Exodus::client_body_size(t_server *t, string line)
{
	string extractedWord = extractmots(line, "client_body_size");
	if (!extractedWord.empty())
		t->client_body_size = strtol(extractedWord.c_str(), NULL, 10);
}

void					Exodus::error_page(t_server *t, string line)
{
	if (recherche(line, "error_page"))
	{
		vector<string> tokens = split(line);
		vector<string>::iterator it = find(tokens.begin(), tokens.end(), "error_page");
		if (it == tokens.end())
			throw logic_error("error_page");
		size_t listenIndex = distance(tokens.begin(), it) + 1;
		if (listenIndex >= tokens.size())
			throw logic_error("error_page") ;
		while (listenIndex < tokens.size() && listenIndex + 1< tokens.size())
		{
			pair <uint32_t, string >	token;
			token.first = strtol(tokens[listenIndex].c_str(), NULL, 10);
			token.second = tokens[listenIndex + 1];
			t->error_pages.push_back(token);
			listenIndex += 2;
		}
	}
	else
		throw logic_error("error_page") ;
}

/**********************************************************************************/
/* -----------------------------------location----------------------------------- */
/**********************************************************************************/

void	Exodus::uri(t_location *t, string line)
{
	string extractedWord = extractmots(line, "location");
    if (!extractedWord.empty())
		t->uri = extractedWord;
}

void					Exodus::allow_methods(t_location *t, string line)
{

	if (recherche(line, "allow_methods"))
	{
		vector<string> tokens = split(line);
		vector<string>::iterator it = find(tokens.begin(), tokens.end(), "allow_methods");
		if (it == tokens.end())
			throw logic_error("allow_methods");
		size_t listenIndex = distance(tokens.begin(), it) + 1;
		if (listenIndex >= tokens.size())
			throw logic_error("allow_methods");
		while (listenIndex < tokens.size())
			t->allow_methods.push_back(tokens[listenIndex++]);
	}
	else
		throw logic_error("allow_methods");

}

void					Exodus::root(t_location *t, string line)
{
	string extractedWord = extractmots(line, "root");
    if (!extractedWord.empty())
		t->root = extractedWord;
}

void					Exodus::index(t_location *t, string line)
{
	if (recherche(line, "index"))
	{
		vector<string> tokens = split(line);
		vector<string>::iterator it = find(tokens.begin(), tokens.end(), "index");
		if (it == tokens.end())
			throw logic_error("index");
		size_t listenIndex = distance(tokens.begin(), it) + 1;
		if (listenIndex >= tokens.size())
			throw logic_error("index");
		while (listenIndex < tokens.size())
			t->index.push_back(tokens[listenIndex++]);
	}
	else
		throw logic_error("index");
}

void	Exodus::redir_link(t_location *t, string line)
{
	string extractedWord = extractmots(line, "redir_link");
    if (!extractedWord.empty())
		t->redir_link = extractedWord;
}

void	Exodus::file_location(t_location *t, string line)
{
	string extractedWord = extractmots(line, "file_location");
    if (!extractedWord.empty())
		t->file_location = extractedWord;
}

void					Exodus::cgi_pass(t_location *t, string line)
{
	string extractedWord = extractmots(line, "cgi_pass");
    if (!extractedWord.empty())
		t->cgi_pass = extractedWord;
}

void					Exodus::directory_listing(t_location *t, string line)
{
	string extractedWord = extractmots(line, "directory_listing");
	if (!extractedWord.empty())
		if (extractedWord == "true")
			t->directory_listing = true;
}

/**********************************************************************************/
/* -----------------------------------exeptions---------------------------------- */
/**********************************************************************************/

const char *Exodus::error_filename::what() const throw()
{
	return (ERROR_FILENAME);
}

const char *Exodus::error_open::what() const throw()
{
	return (ERROR_OPEN);
}

const char *Exodus::error_end::what() const throw()
{
 	return (ERROR_END);
}