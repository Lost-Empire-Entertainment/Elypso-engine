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

namespace ElypsoUtils
{
	class CRASHHANDLER_API Log
	{

	};
}