//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"

namespace KalaLua::Core
{
	using std::string;

	using u32 = uint32_t;

	class LIB_API KalaLuaCore
	{
	public:
		//Force-close the program right this very moment with no cleanups
		[[noreturn]] static void ForceClose(
			const string& target,
			const string& reason);
	};
}