//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <csignal>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

namespace KalaPhysics::Core
{
	using std::string;
	using std::raise;
	
	using u32 = uint32_t;
	
	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;
	using KalaHeaders::KalaLog::TimeFormat;
	using KalaHeaders::KalaLog::DateFormat;

	class LIB_API KalaPhysicsCore
	{
	public:
		//The ID that is bumped by every object when it needs a new ID
		static inline u32 globalID{};
		
		//Run when you want all KalaPhysics resources freed relative to a window ID
		static void CleanAllWindowResources(u32 windowID);
		
		//Run when you want all KalaPhysics resources to be freed
		static void CleanAllResources();
		
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