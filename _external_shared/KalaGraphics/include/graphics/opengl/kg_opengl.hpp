//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>

#include "KalaHeaders/core_utils.hpp"

namespace KalaGraphics::Graphics::OpenGL
{
	using std::string;
	using std::unordered_map;
	
	using u32 = uint32_t;
	
	enum VSyncState
	{
		VSYNC_ON, //Framerate is capped to monitor refresh rate.
		VSYNC_OFF //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
	};
	
	//Per-window GL context
	struct LIB_API WindowGLContext
	{
		VSyncState vsyncState{};
		u32 lastProgramID{};
		
#ifdef _WIN32
		uintptr_t hglrc{}; //OpenGL context wia WGL
		uintptr_t hdc{};   //OpenGL handle to device context
#else
		uintptr_t glxContext{}; //OpenGL context via glx
#endif
	};
	
	class LIB_API OpenGL_Core
	{
	public:
		static void SwapOpenGLBuffers(u32 windowID);

		static void MakeContextCurrent(u32 windowID);
		static bool IsContextValid(u32 windowID);
	
		static void SetOpenGLLibrary();
		static inline uintptr_t GetOpenGLLibrary()
		{
			if (openGL32Lib == NULL) SetOpenGLLibrary();

			return openGL32Lib;
		}
	
		static inline void SetGlobalContext(uintptr_t newValue) { hglrc = newValue; }
		static inline uintptr_t GetGlobalContext() { return hglrc; }
		
		static inline bool GetWindowGLContext(
			u32 windowID,
			WindowGLContext& target)
		{
			if (!windowContexts.contains(windowID)) return false;
			
			target = windowContexts[windowID];
			return true;
		}
		
		//Assign HDC
		static inline void SetHandle(
			u32 windowID,
			uintptr_t newValue)
		{
			if (!windowContexts.contains(windowID)) windowContexts[windowID] = {};
			
			windowContexts[windowID].hdc = newValue;
		}
		//Get HDC
		static inline bool GetHandle(
			u32 windowID,
			uintptr_t& target)
		{
			if (!windowContexts.contains(windowID)
				|| windowContexts[windowID].hdc == NULL)
			{
				return false;
			}
			
			target = windowContexts[windowID].hdc;
			return true;
		}
		
		//Assign HGLRC
		static inline void SetContext(
			u32 windowID,
			uintptr_t newValue)
		{
			if (!windowContexts.contains(windowID)) windowContexts[windowID] = {};
			
			windowContexts[windowID].hglrc = newValue;
		}
		//Get HGLRC
		static inline bool GetContext(
			u32 windowID,
			uintptr_t& target)
		{
			if (!windowContexts.contains(windowID)
				|| windowContexts[windowID].hglrc == NULL)
			{
				return false;
			}
			
			target = windowContexts[windowID].hglrc;
			return true;
		}
	
		static void SetVSyncState(
			u32 windowID,
			VSyncState newValue);
		static inline bool GetVSyncState(
			u32 windowID,
			VSyncState& target)
		{
			if (!windowContexts.contains(windowID)) return false;
			
			target = windowContexts[windowID].vsyncState;
			return true;
		}
		
		static void SetLastProgramID(
			u32 windowID,
			u32 newValue)
		{
			if (!windowContexts.contains(windowID)) windowContexts[windowID] = {};
			
			windowContexts[windowID].lastProgramID = newValue;
		}
		static inline bool GetLastProgramID(
			u32 windowID,
			u32& target)
		{
			if (!windowContexts.contains(windowID)) return false;
			
			target = windowContexts[windowID].lastProgramID;
			return true;
		}
		
		//Place after any gl call to check if an issue or error has occurred within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static string GetError();
	private:
		static inline uintptr_t openGL32Lib{};
		static inline uintptr_t hglrc{}; //master context for shared resources
		
		static inline unordered_map<u32, WindowGLContext> windowContexts{};
	};
}