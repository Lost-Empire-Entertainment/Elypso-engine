//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

namespace KalaGraphics::Graphics::OpenGL
{
	using std::string;
	using std::to_string;
	using std::unordered_map;
	
	using KalaHeaders::Log;
	using KalaHeaders::LogType;
	
	using u32 = uint32_t;
	
	enum VSyncState
	{
		VSYNC_ON, //Framerate is capped to monitor refresh rate.
		VSYNC_OFF //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
	};
	
	//Per-window GL context
	struct LIB_API GLContext
	{
		VSyncState vsyncState{};
		u32 lastProgramID{};
		
#ifdef _WIN32
		uintptr_t hglrc{};       //OpenGL context wia WGL
		uintptr_t parentHglrc{}; //OpenGL shared parent context wia WGL
#else
		uintptr_t glxContext{}; //OpenGL context via glx
#endif
	};
	
	class LIB_API OpenGL_Core
	{
	public:
		//Called at the end of each frame, requires handle (HDC) from your window
		static void SwapOpenGLBuffers(
			u32 glID,
			uintptr_t handle);

		//Make the GL context correct for the current window, requires handle (HDC) from your window
		static void MakeContextCurrent(
			u32 glID,
			uintptr_t handle);
			
		//Confirms that the GL context is the same as the stored context for this window
		static bool IsContextValid(u32 glID);
	
		static void SetOpenGLLibrary();
		static inline uintptr_t GetOpenGLLibrary()
		{
			if (!openGL32Lib) SetOpenGLLibrary();

			return openGL32Lib;
		}
		
		static inline bool AssignGLContext(u32 glID)
		{
			if (glContexts.contains(glID))
			{
				Log::Print(
					"Cannot set new gl context struct with gl ID '" + to_string(glID) + "' because the ID is already assigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
				
				return false;
			}
			
			glContexts[glID] = {};
		
			return true;
		}
		static inline bool GetGLContext(
			u32 glID,
			GLContext& target)
		{
			if (!glContexts.contains(glID))
			{
				Log::Print(
					"Cannot get gl context struct with gl ID '" + to_string(glID) + "' because the ID is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;
			}
			
			target = glContexts[glID];
			
			return true;
		}
		
		//Assign HGLRC
		static inline bool SetContext(
			u32 glID,
			uintptr_t newValue)
		{
			if (!glContexts.contains(glID))
			{
				Log::Print(
					"Cannot set gl context with gl ID '" + to_string(glID) + "' because the ID is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;
			}
			
			glContexts[glID].hglrc = newValue;
			
			return true;
		}
		//Get HGLRC
		static inline bool GetContext(
			u32 glID,
			uintptr_t& target)
		{
			if (!glContexts.contains(glID))
			{
				Log::Print(
					"Cannot get gl context with gl ID '" + to_string(glID) + "' because the ID is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;
			}
			if (!glContexts[glID].hglrc)
			{
				Log::Print(
					"Cannot get gl context with gl ID '" + to_string(glID) + "' because the context is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;	
			}
			
			target = glContexts[glID].hglrc;
			
			return true;
		}
		
		//Assign parent HGLRC for shared resources
		static inline bool SetParentContext(
			u32 glID,
			uintptr_t newValue)
		{
			if (!glContexts.contains(glID))
			{
				Log::Print(
					"Cannot set parent gl context with gl ID '" + to_string(glID) + "' because the ID is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;
			}
			
			glContexts[glID].parentHglrc = newValue;
			
			return true;
		}
		//Get parent HGLRC for shared resources
		static inline bool GetParentContext(
			u32 glID,
			uintptr_t& target)
		{
			if (!glContexts.contains(glID))
			{
				Log::Print(
					"Cannot get parent gl context with gl ID '" + to_string(glID) + "' because the ID is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;
			}
			if (!glContexts[glID].parentHglrc)
			{
				Log::Print(
					"Cannot get parent gl context with gl ID '" + to_string(glID) + "' because the parent context is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;	
			}
			
			target = glContexts[glID].parentHglrc;
			
			return true;
		}
	
		static bool SetVSyncState(
			u32 glID,
			VSyncState newValue);
		static inline bool GetVSyncState(
			u32 glID,
			VSyncState& target)
		{
			if (!glContexts.contains(glID))
			{
				Log::Print(
					"Cannot get vsync state with gl ID '" + to_string(glID) + "' because the ID is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;
			}
			
			target = glContexts[glID].vsyncState;
			
			return true;
		}
		
		static void SetLastProgramID(
			u32 glID,
			u32 newValue)
		{
			if (!glContexts.contains(glID)) glContexts[glID] = {};
			
			glContexts[glID].lastProgramID = newValue;
		}
		static inline bool GetLastProgramID(
			u32 glID,
			u32& target)
		{
			if (!glContexts.contains(glID))
			{
				Log::Print(
					"Cannot get last program ID with gl ID '" + to_string(glID) + "' because the ID is unassigned!",
					"OPENGL",
					LogType::LOG_ERROR,
					2);
			
				return false;
			}
			
			target = glContexts[glID].lastProgramID;
			
			return true;
		}
		
		//Place after any gl call to check if an issue or error has occurred within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static string GetError();
	private:
		static inline uintptr_t openGL32Lib{};
		static inline uintptr_t hglrc{}; //master context for shared resources
		
		static inline unordered_map<u32, GLContext> glContexts{};
	};
}