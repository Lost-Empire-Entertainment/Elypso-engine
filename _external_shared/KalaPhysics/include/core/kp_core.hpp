//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

namespace KalaPhysics::Core
{
	using std::function;
	using std::string;
	using std::abort;
	
	using u32 = uint32_t;
	
	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;

	class LIB_API KalaPhysicsCore
	{
	public:
		//The ID that is bumped by every object in KalaWindow when it needs a new ID
		static inline u32 globalID{};
		
		//Run when you want all KalaPhysics resources freed relative to a window ID,
		//fonts, shaders and textures are not related to one window, they are shared across all windows
		static void CleanAllWindowResources(u32 windowID);
		
		//Run when you want all KalaPhysics resources to be freed
		static void CleanAllResources();
		
		//Calls the force close callback which handles what happens at runtime,
		//always calls __debugbreak in debug, calls abort if no force close callback is assigned
		static inline void SetForceCloseCallback(
			function<void(const string& target, const string& reason)> newCallback)
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
				2);
			
#ifdef _DEBUG
			__debugbreak();
#else
			if (forceCloseCallback) forceCloseCallback(target, reason);
			else quick_exit(EXIT_FAILURE);
#endif
		}
	private:
		static inline function<void(const string& target, const string& reason)> forceCloseCallback{};
	};
}