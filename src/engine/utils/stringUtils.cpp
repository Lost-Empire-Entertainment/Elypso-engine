//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//engine
#include "stringUtils.hpp"
#include "console.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>

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
		try
		{
			size_t pos;
			float convertedValue = stof(value, &pos);

			return pos == value.length();
		}
		catch (const exception& e)
		{
			return false;
		}
	}

	bool String::CanConvertStringToInt(const string& value)
	{
		try
		{
			size_t pos;
			int convertedValue = stoi(value, &pos);

			return pos == value.length();
		}
		catch (const exception& e)
		{
			return false;
		}
	}
}