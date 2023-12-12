#pragma once

#include <string>

using namespace std;

namespace Utils
{
	class String
	{
	public:
		static string Replace(const string& original, const string& search, const string& replacement);
	};
}