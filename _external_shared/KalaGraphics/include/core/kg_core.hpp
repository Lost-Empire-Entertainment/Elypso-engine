//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

namespace KalaGraphics::Core
{
	using std::function;
	using std::string;
	using std::abort;
	
	using u32 = uint32_t;
	
	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;

	class LIB_API KalaGraphicsCore
	{
	public:
		//The ID that is bumped by every object in KalaWindow when it needs a new ID
		static inline u32 globalID{};
		
		//Run when you want all KalaGraphics resources freed relative to a window ID,
		//fonts, shaders and textures are not related to one window, they are shared across all windows
		static void CleanAllWindowResources(u32 windowID);
		
		//Run when you want all KalaGraphics resources to be freed
		static void CleanAllResources();
	
		//Attach a function that is called when you call ForceClose,
		//great for ensuring you clean KalaGraphics resources when you intend the program to close
		static inline void SetErrorCallback(function<void()> newCallback)
		{
			if (!newCallback) return;
			
			errorCallback = newCallback;
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
				2);
			
			if (errorCallback) errorCallback();
			else
			{
#ifdef _DEBUG
			__debugbreak();
#else
			abort();	
#endif
			}
		}
	private:
		static inline function<void()> errorCallback{};
	};
}