//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core_utils.hpp"

namespace ElypsoEngine::Graphics
{
    class LIB_API Render
    {
    public:
        //Initialize graphics system
		static void Initialize(
            bool allowGL = true,
            bool allowVK = false);

        static bool IsInitialized();

        static bool AllowGL();
        static bool AllowVK();

		//Main graphics draw call
		static void Update();

		//Shut down all created graphics content
		static void Shutdown();
    };
}