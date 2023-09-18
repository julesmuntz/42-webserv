#include "utils.hpp"

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