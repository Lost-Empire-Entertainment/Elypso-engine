//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core_utils.hpp"

namespace KalaGraphics::Internal::OpenGL
{
    using u32 = uint32_t;

    enum class VSyncState
	{
		VSYNC_ON, //Framerate is capped to monitor refresh rate.
		VSYNC_OFF //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
	};

    class LIB_API OpenGL
    {
    public:
        static void AddWindow(
            u32 windowID,
            VSyncState vsyncState);
    
        static void SetVSyncState(
            u32 windowID,
            VSyncState newValue);
        static VSyncState GetVSyncState(u32 windowID);

        //Called at the end of each frame
		static void SwapOpenGLBuffers(u32 windowID);
    };
}