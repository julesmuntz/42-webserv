#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

bool recherche(const string& phrase, const string& mots);
vector<string> split(const string& str);
string extractmots(string line, string mots);