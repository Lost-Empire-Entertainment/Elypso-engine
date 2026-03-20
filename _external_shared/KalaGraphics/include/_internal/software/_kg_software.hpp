//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core_utils.hpp"

namespace KalaGraphics::Internal::Software
{
    using u32 = uint32_t;

    class LIB_API Software_Core
    {
    public:
        //Main draw call
        static void Update(u32 windowID);
        //Actions that occur only when the window size changes
        static void ResizeUpdate(u32 windowID);

        //Clean all resources
        static void Shutdown(u32 windowID);
    };
}