//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//external
#include "glfw3.h"

//engine
#include "timeManager.h"
#include "console.h"
#include "input.h"

#include <algorithm>
#include <thread>

using namespace Core;

namespace Core
{
	void TimeManager::UpdateDeltaTime()
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;

		if (Input::printFPSToConsole)
		{
			cout << "Delta Time : " + to_string(deltaTime) << "\n";
		}

		lastFrame = currentFrame;
	}

	void TimeManager::CalculateFPS(bool useMonitorRefreshRate)
	{
		if (!useMonitorRefreshRate)
		{
			static high_resolution_clock::time_point previousFrameTime = high_resolution_clock::now();
			high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();

			auto frameDuration = duration_cast<microseconds>(currentFrameTime - previousFrameTime);
			fps = 1.0 / (frameDuration.count() / 1e6);

			if (Input::printFPSToConsole)
			{
				cout << "Uncapped FPS: " << fps
					 << ", Frame duration: " << frameDuration << "\n";
			}

			previousFrameTime = currentFrameTime;
		}
		else
		{
			//get the current time point
			auto now = high_resolution_clock::now();

			//calculate the elapsed time in seconds
			float elapsed_seconds = duration<float>(now - lastTime).count();

			static high_resolution_clock::time_point previousFrameTime = high_resolution_clock::now();
			high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();
			auto frameDuration = duration_cast<microseconds>(currentFrameTime - previousFrameTime);
			fps = 1.0 / (frameDuration.count() / 1e6);

			//check if the elapsed time is less than the target frame time
			if (elapsed_seconds < targetDT)
			{
				//sleep for the remaining time to avoid busy-waiting
				float sleepDuration = targetDT - elapsed_seconds;
				if (sleepDuration > 0.0f) 
				{
					this_thread::sleep_for(duration<float>(sleepDuration));
				}
			}
			else
			{
				if (Input::printFPSToConsole)
				{
					cout << "targetDT is out of range! " << targetDT << "\n";
				}
			}
			if (Input::printFPSToConsole)
			{
				cout << "Capped FPS: " << fps
					 << ", Elapsed time: " << elapsed_seconds
					 << ", TargetDT: " << TimeManager::targetDT
					 << ", Frame duration: " << frameDuration << "\n";
			}

			previousFrameTime = currentFrameTime;

			//update lastTime for the next iteration
			lastTime = high_resolution_clock::now();
		}

		TimeManager::CalculateDisplayedFPS();
	}

	void TimeManager::CalculateDisplayedFPS()
	{
		high_resolution_clock::time_point currentTime = high_resolution_clock::now();
		auto elapsedSeconds = duration_cast<duration<double>>(currentTime - lastUpdate).count();
		if (elapsedSeconds >= 0.15f)
		{
			lastUpdate = currentTime;
			displayedFPS = fps;
		}
	}
}