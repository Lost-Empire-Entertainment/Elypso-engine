//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef PHYSICS_DLL_EXPORT
		#define PHYSICS_API __declspec(dllexport)  //Export symbols when building the DLL
	#else
		#define PHYSICS_API __declspec(dllimport)  //Import symbols when using the DLL
	#endif
#else
	#define PHYSICS_API
#endif

namespace Core
{
	class Physics
	{
	public:
		static void Test1();
	};
}