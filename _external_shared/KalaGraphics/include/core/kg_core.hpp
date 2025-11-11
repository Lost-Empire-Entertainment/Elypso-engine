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
	
	using KalaHeaders::Log;
	using KalaHeaders::LogType;
	
	class LIB_API KalaGraphicsCore
	{
	public:
		//The ID that is bumped by every object in KalaWindow when it needs a new ID
		static inline u32 globalID{};
	
		static inline function<void()> errorCallback{};
		
		static inline void SetErrorCallback(function<void()> newCallback)
		{
			if (!newCallback) return;
			
			errorCallback = newCallback;
		}
		
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
	};
}