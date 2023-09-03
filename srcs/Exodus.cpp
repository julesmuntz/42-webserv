#include "Exodus.hpp"

bool recherche(const std::string& phrase, const std::string& mots)
{
	size_t pos = phrase.find(mots);

	if (pos != std::string::npos)
		return (phrase.compare(pos, mots.length(), mots) == 0);
	return false;
}

std::vector<std::string> split(const std::string& str) {
	std::vector<std::string> result;
	std::istringstream iss(str);
	std::string token;

	while (iss >> token) {
		result.push_back(token);
	}
	return result;
}

std::string extractmots(std::string line, std::string mots) {
	std::vector<std::string> tokens = split(line);

	std::vector<std::string>::iterator it = std::find(tokens.begin(), tokens.end(), mots);
	if (it == tokens.end())
		return ("");
	size_t listenIndex = std::distance(tokens.begin(), it) + 1;
	if (listenIndex >= tokens.size())
		return ("");
	return tokens[listenIndex];
}

Exodus::Exodus(std::string const filename)
{
	std::string		extantion;

	extantion = FILE_CONF;
	if (filename.size() >= extantion.size() && filename.compare(filename.size() - extantion.size(), extantion.size(), extantion))
		throw Exodus::error_filename();
	this->_ifs.open(filename.c_str(), std::ifstream::in);
	if (!this->_ifs.is_open())
		throw Exodus::error_open();
}

Exodus::~Exodus(){}

std::vector<t_server>	Exodus::get_parss() const
{
	return (this->_server);
}

t_server	Exodus::set_server()
{
	std::string	line;
	t_server	exodus_server;
	bool		end = false;
	std::string servers[] = {"listen" ,"server_name" ,"client_body_size","error_page","allow_methods","root"};
	void		(Exodus::*f[])(t_server *, std::string) = {&Exodus::listen, &Exodus::server_name, &Exodus::client_body_size, &Exodus::error_page, &Exodus::allow_methods, &Exodus::root};

	while (std::getline(this->_ifs, line) && !end)
	{
		if (recherche(line, "}"))
			end = true;
		for(int i = 0; i < static_cast<int>(servers->size()); i++)
		{
			if (recherche(line, servers[i]))
			{
				(this->*f[i])(&exodus_server, line);
				break ;
			}
		}
		if (recherche(line, "location") && recherche(line, "{"))
			exodus_server.location.push_back(set_location());
	}
	if (!end)
		std::cout << "cool" << std::endl;
	return (exodus_server);
}

t_location				Exodus::set_location()
{
	std::string	line;
	t_location	exodus_location;
	std::string	locations[] = {"allow_methods", "root", "index", "cgi_pass", "alias", "client_body_size"};
	void		(Exodus::*f[])(t_location *, std::string) = {&Exodus::allow_methods, &Exodus::root, &Exodus::index, &Exodus::cgi_pass, &Exodus::alias, &Exodus::client_body_size};

	exodus_location.alias = false;
	while (std::getline(this->_ifs, line))
	{
		if (recherche(line, "}"))
			break ;
		for(int i = 0; i < static_cast<int>(locations->size()); i++)
		{
			if (recherche(line, locations[i]))
			{
				(this->*f[i])(&exodus_location, line);
				break ;
			}
		}
	}
	//if (/*fin de fichier { n'est pas alors try chact*/)
	return (exodus_location);
}

void	Exodus::get_Exodus()
{
	std::string line;

	while (std::getline(this->_ifs, line))
	{
		if (recherche(line, "server") && recherche(line, "{"))
			this->_server.push_back(set_server());
	}
}

template<typename T>
void					Exodus::listen(T *t, std::string line)
{
	std::string extractedWord = extractmots(line, "listen");
    if (!extractedWord.empty())
	{
		if (!recherche(extractedWord, ":"))
			throw ;
		size_t delimiterPos = extractedWord.find(":");
		if (delimiterPos != std::string::npos) {
        	t->listens.first = extractedWord.substr(0, delimiterPos);
        	t->listens.second = std::stoi(extractedWord.substr(delimiterPos + 1));
   		} 
		else 
        	throw ;
	}
	else
		throw ;
}

template<typename T>
void					Exodus::server_name(T *t, std::string line)
{
    if (recherche(line, "server_name"))
	{
		std::vector<std::string> tokens = split(line);
		std::vector<std::string>::iterator it = std::find(tokens.begin(), tokens.end(), "server_name");
		if (it == tokens.end())
			throw ;
		size_t listenIndex = std::distance(tokens.begin(), it) + 1;
		if (listenIndex >= tokens.size())
			throw ;
		while (listenIndex < tokens.size())
			t->server_name.push_back(tokens[listenIndex++]);
	}
	else
		throw ;
}

template<typename T>
void					Exodus::client_body_size(T *t, std::string line)
{
	std::string extractedWord = extractmots(line, "client_body_size");
    if (!extractedWord.empty())
		t->client_body_size = std::stoi(extractedWord);
}

template<typename T>
void					Exodus::error_page(T *t, std::string line)
{
    if (recherche(line, "error_page"))
	{
		std::vector<std::string> tokens = split(line);
		std::vector<std::string>::iterator it = std::find(tokens.begin(), tokens.end(), "error_page");
		if (it == tokens.end())
			throw ;
		size_t listenIndex = std::distance(tokens.begin(), it) + 1;
		if (listenIndex >= tokens.size())
			throw ;
		while (listenIndex < tokens.size() && listenIndex + 1< tokens.size())
		{
			std::pair <std::uint32_t, std::string >	token;
			token.first = std::stoi(tokens[listenIndex]);
			token.second = tokens[listenIndex + 1];
			t->error_pages.push_back(token);
			listenIndex += 2;
		}
	}
	else
		throw ;
}

template<typename T>
void					Exodus::allow_methods(T *t, std::string line)
{
	std::string extractedWord = extractmots(line, "allow_methods");
    if (!extractedWord.empty())
		t->allow_methods = extractedWord;
}

template<typename T>
void					Exodus::root(T *t, std::string line)
{
	std::string extractedWord = extractmots(line, "root");
    if (!extractedWord.empty())
		t->root = extractedWord;
}

template<typename T>
void					Exodus::index(T *t, std::string line)
{
	std::string extractedWord = extractmots(line, "index");
    if (!extractedWord.empty())
		t->index = extractedWord;
}
template<typename T>
void					Exodus::cgi_pass(T *t, std::string line)
{
	std::string extractedWord = extractmots(line, "cgi_pass");
    if (!extractedWord.empty())
		t->cgi_pass = extractedWord;
}

template<typename T>
void					Exodus::alias(T *t, std::string line)
{
	(void)line;
	t->alias = true;
}

const char *Exodus::error_filename::what() const throw()
{
	return (ERROR_FILENAME);
}

const char *Exodus::error_open::what() const throw()
{
	return (ERROR_OPEN);
}
