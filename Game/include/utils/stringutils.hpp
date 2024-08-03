//This script is a part of the Game Template that is distributed by Elypso Engine.
//If you have access to this script then you have full freedom to do anything with it.
//A copy of the MIT license has been included in LICENSE.md

#pragma once

#include <vector>
#include <string>
#include <sstream>

//external
#include "glm.hpp"

using std::string;
using std::vector;
using std::istringstream;
using glm::vec3;

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
		/// Convert an inserted vector string to a vec3.
		/// </summary>
		/// <param name="original">A vector of string x, y and z positions.</param>
		/// <returns></returns>
		static vec3 StringToVec3(const vector<string>& original)
		{
			vec3 output{};

			istringstream issX(original[0]);
			istringstream issY(original[1]);
			istringstream issZ(original[2]);

			issX >> output.x;
			issY >> output.y;
			issZ >> output.z;

			return output;
		}

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

		/// <summary>
		/// Check if the character is allowed in paths in Windows
		/// </summary>
		/// <param name="c"></param>
		/// <returns></returns>
		static bool IsValidSymbolInPath(const char& c);
	};
}