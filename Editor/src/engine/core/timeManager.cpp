//Copyright(C) 2025 Lost Empire Entertainment
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

        //how often should displayed framerate update
        smoothing_interval = 0.1; 

        //set to 60 at the start to prevent "slow buildup" to true current fps
        displayedFPS = 60.0;
    }

	void TimeManager::UpdateDeltaTime()
	{
        high_resolution_clock::time_point current_time = high_resolution_clock::now();
        
        duration<double> frame_duration = current_time - last_frame_time;
        last_frame_time = current_time;

        double uncappedDeltaTime = frame_duration.count();

        const double targetFPS = 60.0;
        const double targetFrameTime = 1.0 / targetFPS;

        deltaTime = min(uncappedDeltaTime, targetFrameTime);

        smoothed_frame_count++;

        duration<double> elapsed_seconds = current_time - last_smoothed_update;
        double elapsedTimeInSeconds = elapsed_seconds.count();

        if (elapsedTimeInSeconds >= smoothing_interval)
        {
            double currentFPS = smoothed_frame_count / elapsedTimeInSeconds;

            double smoothingFactor = 0.5;
            displayedFPS = (smoothingFactor * displayedFPS) + ((1.0 - smoothingFactor) * currentFPS);

            smoothed_frame_count = 0;
            last_smoothed_update = current_time;
        }
	}
}