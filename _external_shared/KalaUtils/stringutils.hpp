//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAUTILS_DLL_EXPORT
		#define KALAUTILS_API __declspec(dllexport)
	#else
		#define KALAUTILS_API __declspec(dllimport)
	#endif
#else
	#define KALAUTILS_API
#endif

#include <string>
#include <vector>

namespace KalaKit
{
	using std::string;
	using std::vector;

	/// <summary>
	/// Defines a simple vec3 struct instead of including the entire glm library just for string utils.
	/// </summary>
	struct kvec3
	{
		float x;
		float y;
		float z;
	};

	class KALAUTILS_API StringUtils
	{
	public:
		/// <summary>
		/// Replace a part of a string with something else.
		/// </summary>
		/// <param name="original">Full string</param>
		/// <param name="search">Part to search for.</param>
		/// <param name="replacement">Part to replace searched part with.</param>
		static string StringReplace(const string& original, const string& search, const string& replacement);

		/// <summary>
		/// Replace a part of a char with something else.
		/// </summary>
		/// <param name="original"></param>
		/// <param name="search"></param>
		/// <param name="replacement"></param>
		static string CharReplace(const string& original, const char& search, const char& replacement);

		/// <summary>
		/// Convert an inserted vector string to a vec3.
		/// </summary>
		/// <param name="original">A vector of string x, y and z positions.</param>
		static kvec3 StringToVec3(const vector<string>& original);

		/// <summary>
		/// Split a string in two from the delimiter.
		/// </summary>
		/// <param name="input">Full string.</param>
		/// <param name="delimiter">Which char is the splitter?</param>
		static vector<string> Split(const string& input, char delimiter);

		/// <summary>
		/// Remove everything except the selected instances.
		/// </summary>
		/// <param name="originalVector">The original vector we are editing</param>
		/// <param name="instance">The part that should be kept.</param>
		static vector<string> RemoveExcept(const vector<string>& originalVector, const string& instance);

		/// <summary>
		/// Find and remove all duplicates of already existing strings in the string vector.
		/// </summary>
		/// <param name="originalVector">The original vector we are editing.</param>
		static vector<string> RemoveDuplicates(const vector<string>& originalVector);

		static bool CanConvertStringToFloat(const string& value);

		static bool CanConvertStringToInt(const string& value);

		/// <summary>
		/// Check if the character is allowed in paths in Windows
		/// </summary>
		/// <param name="c"></param>
		static bool IsValidSymbolInPath(const char& c);
	};
}