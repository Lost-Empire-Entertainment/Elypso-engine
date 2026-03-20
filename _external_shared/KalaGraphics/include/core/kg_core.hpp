//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

#include "core_utils.hpp"

namespace KalaGraphics::Core
{
    using std::string_view;

    using u32 = uint32_t;

    class LIB_API KalaGraphicsCore
    {
    public:
        static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);

        //Force-closes the application and gives a breakpoint, good for hard stops or bad user errors
		static void ForceClose(
			string_view title,
			string_view reason);
    };
}