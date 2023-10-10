#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include "struct.hpp"

using namespace std;

bool			is_positive_hexa(std::string &str);
bool			is_positive_deci(std::string &str);
bool			check_overflow(std::string &str);
bool			recherche(const string& phrase, const string& mots);
vector<string>	split(const string& str);
string			extractmots(string line, string mots);
bool			isFile(std::string &path);
bool			isDir(std::string &path);
bool			isWrite(std::string &path);
bool			cmp(const t_location a, const t_location b);
std::string		uint32_to_string(uint32_t value);
std::string		max_chars(const char *input, size_t cap);
string			get_path(string uri, t_location &loc);