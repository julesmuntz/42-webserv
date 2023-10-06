#include "utils.hpp"
#include <string.h>
/**********************************************************************************/
/* ---------------------------------utils---------------------------------------- */
/**********************************************************************************/

bool	is_positive_hexa(std::string &str)
{
	if (str.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos)
		return (true);
	return (false);
}

bool	is_positive_deci(std::string &str)
{
	if (str.find_first_not_of("0123456789") == std::string::npos)
		return (true);
	return (false);
}

bool	check_overflow(std::string &str)
{
	if (str[0] == '0')
		str.erase(0, str.find_first_not_of("0"));
	if (str.size() > 9)
	{
		if (atol(str.c_str()) > 2147483647)
			return (true);
	}
	return (false);
}

bool	recherche(const string& phrase, const string& mots)
{
	size_t pos = string::npos;
	pos = phrase.find(mots);

	if (pos == string::npos)
		return false;
	return (phrase.compare(pos, mots.length(), mots) == 0);
}

vector<string>	split(const string& str) {
	vector<string> result;
	istringstream iss(str);
	string token;

	while (iss >> token) {
		result.push_back(token);
	}
	return result;
}

string	extractmots(string line, string mots) {
	vector<string> tokens = split(line);

	vector<string>::iterator it = find(tokens.begin(), tokens.end(), mots);
	if (it == tokens.end())
		return ("");
	size_t listenIndex = distance(tokens.begin(), it) + 1;
	if (listenIndex >= tokens.size())
		return ("");
	return tokens[listenIndex];
}

bool isFile(std::string &path)
{

	struct stat path_stat;

	bzero(&path_stat, sizeof(path_stat));
	stat(path.c_str(), &path_stat);
	return (S_ISREG(path_stat.st_mode));
}

bool isDir(std::string &path)
{

	struct stat path_stat;

	bzero(&path_stat, sizeof(path_stat));
	if (stat(path.c_str(), &path_stat) != 0)
		return (false);
	return (S_ISDIR(path_stat.st_mode));
}

bool	isWrite(std::string &path)
{
	struct stat path_stat;

	bzero(&path_stat, sizeof(path_stat));
	if (stat(path.c_str(), &path_stat))
		return (false);
	if (path_stat.st_mode & S_IWUSR)
		return (true);
	return (false);
}

bool	cmp(const t_location a, const t_location b)
{
	return (a.uri.size() < b.uri.size());
}

std::string uint32_to_string(uint32_t value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string max_chars(const char *input, size_t cap)
{
	std::string result(cap, '\0');
	strncpy(&result[0], input, cap);
	return result;
}
