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
#include "timeManager.hpp"
#include "input.hpp"

#include <algorithm>
#include <thread>

using namespace Core;

namespace Core
{
	void TimeManager::UpdateDeltaTime()
	{
		//get the current time in seconds
		float currentFrame = static_cast<float>(glfwGetTime());

		//calculate the frame duration since the last frame
		deltaTime = currentFrame - lastFrame;

		//cap delta time to a maximum value (e.g., 1.0 / 60 seconds) to prevent large time steps
		deltaTime = std::min(deltaTime, 1.0f / 60);

		//update last frame time to keep track of delta time
		lastFrame = currentFrame;

		//calculate elapsed time and FPS
		auto now = high_resolution_clock::now();
		float elapsed_seconds = duration<float>(now - lastTime).count();

		static high_resolution_clock::time_point previousFrameTime = high_resolution_clock::now();
		high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();

		auto frameDuration = duration_cast<microseconds>(currentFrameTime - previousFrameTime);
		fps = 1.0 / (frameDuration.count() / 1e6);
		previousFrameTime = currentFrameTime;
		lastTime = high_resolution_clock::now();

		//calculate displayed FPS
		high_resolution_clock::time_point currentTime = high_resolution_clock::now();
		auto elapsedSeconds = duration_cast<duration<double>>(currentTime - lastUpdate).count();
		if (elapsedSeconds >= 0.15f)
		{
			lastUpdate = currentTime;
			displayedFPS = fps;

			//print FPS to the console if enabled
			if (Input::printFPSToConsole)
			{
				cout << "FPS: " << displayedFPS
					<< ", Elapsed time: " << elapsed_seconds
					<< ", TargetDT: " << TimeManager::targetDT
					<< ", Frame duration: " << frameDuration
					<< ", Delta Time: " + to_string(deltaTime) << "\n";
			}
		}
	}
}