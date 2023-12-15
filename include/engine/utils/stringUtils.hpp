#pragma once

namespace Utils
{
	class String
	{
	public:
		/// <summary>
		/// Replace a part of a string with something else.
		/// </summary>
		/// <param name="original">Full string.</param>
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