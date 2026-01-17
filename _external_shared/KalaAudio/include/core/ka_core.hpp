//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"

namespace KalaAudio::Core
{
	using std::string;
	
	using u32 = uint32_t;
	
	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;
	using KalaHeaders::KalaLog::TimeFormat;
	using KalaHeaders::KalaLog::DateFormat;
	
	class LIB_API KalaAudioCore
	{
	public:
		static void SetGlobalID(u32 newID);
		static u32 GetGlobalID();
		
		//Run when you want all KalaAudio resources freed relative to a window ID
		static void CleanAllWindowResources(u32 windowID);
		
		//Run when you want all KalaAudio resources to be freed
		static void CleanAllResources();
		
		//Force-close the program right this very moment with no cleanups
		[[noreturn]] static void ForceClose(
			const string& target,
			const string& reason);
	};
}