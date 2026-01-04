//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <csignal>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

namespace KalaLua::Core
{
	using std::string;
	using std::raise;

	using u32 = uint32_t;

	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;
	using KalaHeaders::KalaLog::TimeFormat;
	using KalaHeaders::KalaLog::DateFormat;

	class LIB_API KalaLuaCore
	{
	public:
		//Force-close the program right this very moment with no cleanups
		[[noreturn]] static inline void ForceClose(
			const string& target,
			const string& reason)
		{
			Log::Print(
				"\n================"
				"\nFORCE CLOSE"
				"\n================\n",
				true);

			Log::Print(
				reason,
				target,
				LogType::LOG_ERROR,
				2,
				true,
				TimeFormat::TIME_NONE,
				DateFormat::DATE_NONE);

#ifdef _WIN32
			__debugbreak();
#else
			raise(SIGTRAP);
#endif
		}
	};
}