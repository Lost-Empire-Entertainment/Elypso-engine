//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef CRASHHANDLER_DLL_EXPORT
		#define CRASHHANDLER_API __declspec(dllexport)
	#else
		#define CRASHHANDLER_API __declspec(dllimport)
	#endif
#else
	#define CRASHHANDLER_API
#endif

#include <string>

namespace ElypsoUtils
{
	using std::string;

	class CRASHHANDLER_API ErrorPopup
	{
	public:
		/// <summary>
		/// Internal error popup function, not meant to be used by end user.
		/// </summary>
		/// <param name="message"></param>
		static void CreateErrorPopup(const string& message);
	};
}