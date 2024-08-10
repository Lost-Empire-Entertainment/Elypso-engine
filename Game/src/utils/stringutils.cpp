//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <fstream>
#include <algorithm>

//game
#include "stringutils.hpp"
#include "console.hpp"

using std::stof;
using std::stoi;
using std::ifstream;
using std::endl;
using std::cerr;
using std::exception;
using std::istringstream;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Utils
{
	string String::StringReplace(const string& original, const string& search, const string& replacement)
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

	string String::CharReplace(const string& original, const char& search, const char& replacement)
	{
		string result = original;
		size_t pos = 0;
		while ((pos = result.find(search, pos)) != string::npos)
		{
			result[pos] = replacement;
			pos += 1;
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

	vector<string> String::RemoveExcept(const vector<string>& originalVector, const string& instance)
	{
		auto containsInstance = [&instance](const string& s)
			{
				return s.find(instance) != string::npos;
			};

		//check if any element contains the specified instance
		bool foundInstance = any_of(originalVector.begin(), originalVector.end(), containsInstance);

		if (!foundInstance)
		{
			return originalVector;
		}

		//create a new vector with elements not containing the specified instance
		vector<string> modifiedVector;
		copy_if(
			originalVector.begin(),
			originalVector.end(),
			back_inserter(modifiedVector),
			containsInstance);
		{
			return modifiedVector;
		};
	}

	vector<string> String::RemoveDuplicates(const vector<string>& originalVector)
	{
		//create a copy of the original vector
		vector<string> modifiedVector = originalVector;

		//sort the vector to bring duplicates together
		sort(modifiedVector.begin(), modifiedVector.end());

		//remove adjacent duplicates
		modifiedVector.erase(
			unique(modifiedVector.begin(),
				modifiedVector.end()),
			modifiedVector.end());

		return modifiedVector;
	}

	bool String::ContainsString(const string& filePath, const string& targetString)
	{
		ifstream file(filePath);
		if (!file.is_open())
		{
			cerr << "Error opening file: " << filePath << endl;
			return false;
		}

		string line;
		while (getline(file, line)) {
			if (line.find(targetString) != string::npos)
			{
				file.close();
				return true;
			}
		}

		file.close();
		return false;
	}

	bool String::CanConvertStringToFloat(const string& value)
	{
		size_t pos;
		float convertedValue = stof(value, &pos);

		return pos == value.length();
	}

	bool String::CanConvertStringToInt(const string& value)
	{
		size_t pos;
		int convertedValue = stoi(value, &pos);

		return pos == value.length();
	}

	bool String::IsValidSymbolInPath(const char& c)
	{
		return c == '-'
			|| c == '_'
			|| c == ' '
			|| (c >= '0' && c <= '9')
			|| (c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z');
	}
}