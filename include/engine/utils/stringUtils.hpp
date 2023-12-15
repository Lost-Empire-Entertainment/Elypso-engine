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
		static string Replace(const string& original, const string& search, const string& replacement);

		/// <summary>
		/// Split a string in two from the delimiter.
		/// </summary>
		/// <param name="input">Full string.</param>
		/// <param name="delimiter">Which char is the splitter?</param>
		/// <returns></returns>
		static vector<string> Split(const string& input, char delimiter);
	};
}