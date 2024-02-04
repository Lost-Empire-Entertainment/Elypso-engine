//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//external
#include "glfw3.h"

//engine
#include "input.hpp"
#include "timeManager.hpp"
#include "console.hpp"

#include <iostream>
#include <thread>

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