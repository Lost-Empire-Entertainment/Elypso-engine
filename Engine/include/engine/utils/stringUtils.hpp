//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#pragma once

#include <vector>
#include <string>

using std::string;
using std::vector;

namespace Utils
{
	class String
	{
	public:
		/// <summary>
		/// Replace a part of a string with something else.
		/// </summary>
		/// <param name="original">Full string</param>
		/// <param name="search">Part to search for.</param>
		/// <param name="replacement">Part to replace searched part with.</param>
		/// <returns></returns>
		static string StringReplace(const string& original, const string& search, const string& replacement);

		/// <summary>
		/// Replace a part of a char with something else.
		/// </summary>
		/// <param name="original"></param>
		/// <param name="search"></param>
		/// <param name="replacement"></param>
		/// <returns></returns>
		static string CharReplace(const string& original, const char& search, const char& replacement);

		/// <summary>
		/// Split a string in two from the delimiter.
		/// </summary>
		/// <param name="input">Full string.</param>
		/// <param name="delimiter">Which char is the splitter?</param>
		/// <returns></returns>
		static vector<string> Split(const string& input, char delimiter);

		/// <summary>
		/// Remove everything except the selected instances.
		/// </summary>
		/// <param name="originalVector">The original vector we are editing</param>
		/// <param name="instance">The part that should be kept.</param>
		/// <returns></returns>
		static vector<string> RemoveExcept(const vector<string>& originalVector, const string& instance);

		/// <summary>
		/// Find and remove all duplicates of already existing strings in the string vector.
		/// </summary>
		/// <param name="originalVector">The original vector we are editing.</param>
		/// <returns></returns>
		static vector<string> RemoveDuplicates(const vector<string>& originalVector);

		/// <summary>
		/// Check if the selected file contains the selected string.
		/// </summary>
		/// <param name="filePath">Where is the file located?</param>
		/// <param name="targetString">What is the string you are looking for?</param>
		/// <returns></returns>
		static bool ContainsString(const string& filePath, const string& targetString);

		static bool CanConvertStringToFloat(const string& value);

		static bool CanConvertStringToInt(const string& value);
	};
}