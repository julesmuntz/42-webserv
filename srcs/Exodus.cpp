#include "Exodus.hpp"

/**********************************************************************************/
/* -------------------------constructeur destructeur----------------------------- */
/**********************************************************************************/

size_t getFilesize(const std::string& filename) {
	struct stat st;
	if(stat(filename.c_str(), &st) != 0) {
		return 0;
	}
	return st.st_size;
}

Exodus::Exodus(string const filename)
{
	string		extension;
	file_error = false;

	extension = FILE_CONF;
	if ((filename.size() >= extension.size() && filename.compare(filename.size() - extension.size(), extension.size(), extension)) ||
		(filename.size() < extension.size()))
		file_error = true;
	if (!file_error && (getFilesize(filename) == 0 || getFilesize(filename) > 10000))
		file_error = true;
	if (!file_error)
	{
		this->_ifs.open(filename.c_str(), ifstream::in);
		if (!this->_ifs.is_open())
			file_error = true;
	}
}

Exodus::~Exodus()
{

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

void	Exodus::failure()
{
	cout << "Config par default" << endl;
	std::string defaults;
	defaults =    "server {\n"
    "    listen localhost:8000;\n"
    "    server_name localhost;\n"
    "    client_body_size 2000000000;\n"
    "    error_page 404 /error/404.html;\n"
    "    location / {\n"
    "        allow_methods POST DELETE GET;\n"
    "        root html;\n"
    "        index index.html index.php;\n"
    "        file_location static;\n"
    "        directory_listing false;\n"
    "    }\n"
    "    location /google {\n"
    "        allow_methods GET;\n"
    "        redir_link https://google.fr;\n"
    "}\n"
	"}";
	_ss.str(defaults);
	string line;
	while (getline(_ss, line))
	{
		if (recherche(line, "server") && recherche(line, "{"))
			this->_server.push_back(set_server_def());
	}
}

void	Exodus::setup()
{
	string line;
	if (file_error)
		return (this->_server.clear() ,this->failure());
	while (getline(this->_ifs, line))
	{
		if (recherche(line, "server") && recherche(line, "{"))
			this->_server.push_back(set_server());
	}
	if (this->_ifs.is_open())
		this->_ifs.close();
	if (this->_server.empty())
		return (this->_server.clear() ,this->failure());
	for(long unsigned int i = 0; i < this->_server.size(); i++)
	{
		if (this->_server[i].listen.first == 0 || this->_server[i].listen.second.empty() || this->_server[i].server_name.empty() || this->_server[i].client_body_size == 0)
			return (this->_server.clear() ,this->failure());
		for(long unsigned int y = 0; y < this->_server[i].location.size(); y++)
		{
			if (this->_server[i].location[y].redir_link.empty())
				if (this->_server[i].location[y].uri.empty() || this->_server[i].location[y].allow_methods.empty() || this->_server[i].location[y].root.empty() || this->_server[i].location[y].index.empty() || this->_server[i].location[y].file_location.empty())
					return (this->_server.clear() ,this->failure());
		}
	}
}

/**********************************************************************************/
/* -------------------------seteur server location------------------------------- */
/**********************************************************************************/

t_server	Exodus::set_server_def()
{
	string	line;
	t_server	exodus_server;
	string servers[4] = {"listen" ,"server_name" ,"client_body_size","error_page"};
	void		(Exodus::*f[4])(t_server *, string) = {&Exodus::listen, &Exodus::server_name, &Exodus::client_body_size, &Exodus::error_page};
	bool		end = false;

	while (!end && getline(_ss, line))
	{
		if (recherche(line, "location") && recherche(line, "{"))
			exodus_server.location.push_back(set_location_def(line));
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

t_location				Exodus::set_location_def(string line)
{
	bool		end = false;
	t_location	exodus_location;
	string	locations[8] = {"uri", "allow_methods", "root", "redir_link", "index", "directory_listing", "file_location", "cgi_path"};
	void		(Exodus::*f[8])(t_location *, string) = {&Exodus::uri, &Exodus::allow_methods, &Exodus::root, &Exodus::redir_link, &Exodus::index, &Exodus::directory_listing, &Exodus::file_location, &Exodus::cgi_path};

	exodus_location.directory_listing = false;

	uri(&exodus_location, line);
	while (!end && getline(_ss, line) )
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

t_location				Exodus::set_location(string line)
{
	bool		end = false;
	t_location	exodus_location;
	string	locations[8] = {"uri", "allow_methods", "root", "redir_link", "index", "directory_listing", "file_location", "cgi_path"};
	void		(Exodus::*f[8])(t_location *, string) = {&Exodus::uri, &Exodus::allow_methods, &Exodus::root, &Exodus::redir_link, &Exodus::index, &Exodus::directory_listing, &Exodus::file_location, &Exodus::cgi_path};

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
			string port = extractedWord.substr(delimiterPos + 1);
			if (!is_positive_deci(port))
				throw logic_error("listen port");
			t->listen.first = strtol(port.c_str(), NULL, 10);
			if (t->listen.first > 65535)
				throw logic_error("listen port");
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
		while (listenIndex < tokens.size() && listenIndex + 1 < tokens.size())
		{
			t->error_pages.insert(pair<uint32_t, std::string>(strtol(tokens[listenIndex].c_str(), NULL, 10), tokens[listenIndex + 1]));
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
			t->allow_methods.insert(pair<string, string>(tokens[listenIndex++], ""));
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

void					Exodus::cgi_path(t_location *t, string line)
{
	string extractedWord = extractmots(line, "cgi_path");
	if (!extractedWord.empty())
		t->cgi_path = extractedWord;
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
