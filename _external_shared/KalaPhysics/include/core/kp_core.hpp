//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"

namespace KalaPhysics::Core
{
	using std::string;

	using u32 = uint32_t;
	
	class LIB_API KalaPhysicsCore
	{
	public:
		static void SetGlobalID(u32 newID);
		static u32 GetGlobalID();
		
		//Run when you want all KalaPhysics resources freed relative to a window ID
		static void CleanAllWindowResources(u32 windowID);
		
		//Run when you want all KalaPhysics resources to be freed
		static void CleanAllResources();
		
		//Force-close the program right this very moment with no cleanups
		static void ForceClose(
			const string& target,
			const string& reason);
	};
}