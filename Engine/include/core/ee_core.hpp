//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

#include "core_utils.hpp"

namespace ElypsoEngine::Core
{
    using std::string_view;

    class LIB_API EngineCore
    {
    public:
        //Core engine initialization
        static void Initialize(string_view programName);

        static bool IsInitialized();

        //Synchronizes all library IDs
        static void SyncID();
        
        //Shut down engine
        static void Shutdown();  
    };
}