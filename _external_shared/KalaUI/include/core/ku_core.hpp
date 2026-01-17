//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"

namespace KalaUI::Core
{
	using std::string;

	using u32 = uint32_t;

	class LIB_API KalaUICore
	{
	public:
		static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);
		
		//Run when you want all KalaUI resources freed relative to a window ID,
		//fonts, shaders and textures are not related to one window, they are shared across all windows
		static void CleanAllWindowResources(u32 windowID);
		
		//Run when you want all KalaUI resources to be freed
		static void CleanAllResources();
		
		//Force-close the program right this very moment with no cleanups
		[[noreturn]] static void ForceClose(
			const string& target,
			const string& reason);
	};
}