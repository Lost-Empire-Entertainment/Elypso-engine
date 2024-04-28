//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <thread>

//external
#include "glfw3.h"

//engine
#include "input.hpp"
#include "timeManager.hpp"
#include "console.hpp"

using std::cout;
using std::to_string;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;
using std::min;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
    TimeManager timeManager;

    void TimeManager::InitializeDeltaTime()
    {
        timeManager.start_time = high_resolution_clock::now();
        timeManager.last_frame_time = timeManager.start_time;

        timeManager.last_smoothed_update = timeManager.last_frame_time;

        //how often should displayed framerate update
        timeManager.smoothing_interval = 0.1; 
    }

	void TimeManager::UpdateDeltaTime()
	{
        high_resolution_clock::time_point current_time = 
            high_resolution_clock::now();
        timeManager.frame_duration = 
            current_time 
            - timeManager.last_frame_time;
        deltaTime = timeManager.frame_duration.count();

        const double targetFPS = 60.0;
        const double targetFrameTime = 1.0 / targetFPS;

        deltaTime = min(timeManager.frame_duration.count(), targetFrameTime);

        timeManager.last_frame_time = current_time;

        timeManager.smoothed_frame_count++;

        current_time = high_resolution_clock::now();
        timeManager.elapsed_seconds = 
            current_time - 
            timeManager.last_smoothed_update;
        if (timeManager.elapsed_seconds.count() 
            >= timeManager.smoothing_interval)
        {
            displayedFPS = 
                static_cast<double>(timeManager.smoothed_frame_count) 
                / timeManager.elapsed_seconds.count();
            timeManager.smoothed_frame_count = 0;
            timeManager.last_smoothed_update = current_time;
        }
	}
}