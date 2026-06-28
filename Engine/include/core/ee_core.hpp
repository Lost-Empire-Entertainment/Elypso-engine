//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "core_utils.hpp"
#include "math_utils.hpp"

namespace ElypsoEngine::Core
{
    using KalaHeaders::KalaMath::vec2;

    using std::string;

    struct LIB_API AppConfig
    {
        string title = "UNASSIGNED TITLE";
        vec2 pos{};
        vec2 size = { 800, 600 };
    };

    extern const AppConfig appConfig;

    extern void Init();
    extern void Update();

    class LIB_API EngineCore
    {
    public:
        //Synchronizes all library IDs
        static void SyncID();
        
        //Shut down engine
        static void Shutdown();
    };
}