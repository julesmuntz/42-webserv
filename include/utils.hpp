#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

bool			is_positive_hexa(std::string &str);
bool			is_positive_deci(std::string &str);
bool			check_overflow(std::string &str);
bool			recherche(const string& phrase, const string& mots);
vector<string>	split(const string& str);
string			extractmots(string line, string mots);
