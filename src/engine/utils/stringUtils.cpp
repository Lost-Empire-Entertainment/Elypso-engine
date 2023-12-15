//engine
#include "stringUtils.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

using std::string;
using std::vector;
using std::istringstream;

namespace Utils
{
	string String::Replace(const string& original, const string& search, const string& replacement)
	{
		string result = original;
		size_t pos = 0;
		while ((pos = result.find(search, pos)) != string::npos)
		{
			result.replace(pos, search.length(), replacement);
			pos += replacement.length();
		}
		return result;
	}

	vector<string> String::Split(const string& input, char delimiter)
	{
		vector<string> tokens;
		string token;
		istringstream tokenStream(input);
		while (getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}
}