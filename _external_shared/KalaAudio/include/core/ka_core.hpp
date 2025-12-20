//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

namespace KalaAudio::Core
{
	using std::function;
	using std::string;
	
	using u32 = uint32_t;
	
	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;
	using KalaHeaders::KalaLog::TimeFormat;
	using KalaHeaders::KalaLog::DateFormat;
	
	class LIB_API KalaAudioCore
	{
	public:
		//The ID that is bumped by every object when it needs a new ID
		static inline u32 globalID{};
		
		//Run when you want all KalaAudio resources freed relative to a window ID
		static void CleanAllWindowResources(u32 windowID);
		
		//Run when you want all KalaAudio resources to be freed
		static void CleanAllResources();
		
		//Calls the force close callback which handles what happens at runtime,
		//always calls __debugbreak in debug, calls quick_exit if no force close callback is assigned
		static inline void SetForceCloseCallback(
			function<void(const string& target,
			const string& reason)> newCallback)
		{
			if (!newCallback) return;
			
			forceCloseCallback = newCallback;
		}
		
		//Force-close the program right this very moment with no cleanups
		[[noreturn]] static inline void ForceClose(
			const string& target,
			const string& reason)
		{
			Log::Print(
				reason,
				target,
				LogType::LOG_ERROR,
				2,
				true,
				TimeFormat::TIME_NONE,
				DateFormat::DATE_NONE);
			
#ifdef _DEBUG
			__debugbreak();
#else
			if (forceCloseCallback) forceCloseCallback(target, reason);
			else quick_exit(EXIT_FAILURE);
#endif
		}
	private:
		static inline function<void(
			const string& target, 
			const string& reason)>
			forceCloseCallback{};
	};
}