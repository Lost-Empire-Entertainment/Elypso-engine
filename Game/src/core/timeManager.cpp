//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <thread>

//external
#include "glfw3.h"

//game
#include "input.hpp"
#include "timeManager.hpp"

using std::cout;
using std::to_string;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;
using std::min;

namespace Core
{
    void TimeManager::InitializeDeltaTime()
    {
        start_time = high_resolution_clock::now();
        last_frame_time = start_time;

        last_smoothed_update = last_frame_time;
        smoothing_interval = 0.1; //how often should displayed framerate update
    }

	void TimeManager::UpdateDeltaTime()
	{
        high_resolution_clock::time_point current_time = high_resolution_clock::now();
        frame_duration = current_time - last_frame_time;
        deltaTime = frame_duration.count();

        const double targetFPS = 60.0;
        const double targetFrameTime = 1.0 / targetFPS;

        deltaTime = min(frame_duration.count(), targetFrameTime);

        last_frame_time = current_time;

        smoothed_frame_count++;

        current_time = high_resolution_clock::now();
        elapsed_seconds = current_time - last_smoothed_update;
        if (elapsed_seconds.count() >= smoothing_interval)
        {
            displayedFPS = static_cast<double>(smoothed_frame_count) / elapsed_seconds.count();
            smoothed_frame_count = 0;
            last_smoothed_update = current_time;
        }
	}
}