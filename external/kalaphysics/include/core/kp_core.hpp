//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "core_utils.hpp"

namespace KalaPhysics::Core
{
	using std::string;
	
	class LIB_API KalaPhysicsCore
	{
	public:
		KNODISCARD
		static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);
		
		//Force-close the program right this very moment with no cleanups
		KNORETURN
		static void ForceClose(
			const string& target,
			const string& reason);
	};
}