//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <filesystem>
#include <functional>

#include "math_utils.hpp"

namespace ElypsoEngine::Graphics
{
    class EngineWindow;
}

namespace ElypsoEngine::Core
{
    using KalaHeaders::KalaMath::vec2;

    using std::string;
    using std::filesystem::path;
    using std::function;

    struct LIB_API AppConfig
    {
        //Main window title
        string title = "UNASSIGNED TITLE";
        //Main window position
        vec2 pos{};
        //Main window size
        vec2 size = { 800, 600 };
    };

    extern const AppConfig appConfig;

    //Runs once before any update calls
    extern void Init();

    //Runs once per frame before the general content of the Engine update loop
    extern void EarlyUpdate();
    //Runs once per frame, locked to 60 fps regardless of application speed
    extern void FixedUpdate();
    //The general update call, runs once per frame
    extern void Update();
    //Runs once per frame after most Engine-side updates are complete
    extern void LateUpdate();
    //Runs once just before application shutdown,
    //should not contain KalaWindow window-specific operations because
    //user may trigger engine shutdown via X button or Alt+F4
    //which closes the process windows first before this call can be reached
    extern void Shutdown();

    class LIB_API EngineCore
    {
    friend class ElypsoEngine::Graphics::EngineWindow;
    public:
        //Synchronizes all library IDs
        static void SyncID();

        //Smooth timer for preventing chaotic explosions
        //or telerportations after a long stall
        KNODISCARD
		static f64 GetDeltaTime();
        //Real time between each frame
        KNODISCARD
		static f64 GetFrameTime();

        KNODISCARD
        static u16 GetFrameTimeSampleCount();
        //Adjust the stored amount of samples for measuring 1% and 0.1% lows,
        //cannot be set below 1000, defaults to 1000
        static void SetFrameTimeSampleCount(u16 newValue);

        //Get average fps over 0.5 seconds 
        KNODISCARD
		static f64 GetAverageFPS();
        //Get average frame length over 0.5 seconds in milliseconds
        KNODISCARD
        static f64 GetAverageFrameLength();
        KNODISCARD
		static f64 GetOnePercentLowFPS();
        KNODISCARD
		static f64 GetZeroPointOnePercentLowFPS();

        //Lock a specific file so another process or user cannot accidentally modify, move or delete it
        static void LockFile(const path& file);
        //Unlock a file locked by Elypso Engine
        static void UnlockFile(const path& file);
        //Unlock all files locked by Elypso Engine
        static void UnlockAllLockedFiles();

		//Assign a callback that gets triggered to check if the engine is allowed to close,
		//your callback should return false if you dont want to allow the engine to close,
        //overrides all process window can close callbacks to this one
        static void SetCanCloseCallback(function<bool()> canCloseCallback);
        //Returns true if canCloseCallback is not assigned or if it returns true
        static bool CanClose();
        
        //Shut down engine
        static void Shutdown();
    private:
        static bool HasCalledEngineShutdown();
        static function<bool()> GetCanCloseCallback();
    };
}