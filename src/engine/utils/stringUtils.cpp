//engine
#include "stringUtils.hpp"

#include <iostream>
#include <string>

using namespace std;

namespace Utils
{
	/// <summary>
	/// Replace a part of a string with something else.
	/// </summary>
	/// <param name="original">Full string</param>
	/// <param name="search">Part to search for.</param>
	/// <param name="replacement">Part to replace searched part with.</param>
	/// <returns></returns>
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
}