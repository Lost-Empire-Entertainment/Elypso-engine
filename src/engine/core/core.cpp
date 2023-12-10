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
#include "glad.h"

//engine
#include "core.h"
#include "deltaTime.h"
#include "admin.h"
#include "console.h"
#include "render.h"
#include "shutdown.h"

#include <string>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace std::chrono;
using namespace Graphics;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void Engine::InitializeEngine()
	{
		name = "Elypso engine";
		version = "0.0.5 Prototype";
		targetUpdateInterval = 0.1;

		cout << "Copyright(C) < 2023 > < Greenlaser >\n";
		cout << "Initializing " + name + " " + version + "...\n\n";

		Admin::RunAsAdmin();

		Render::RenderSetup();
	}

	void Engine::RunEngine()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_LOOP,
			Type::INFO,
			"Entering window loop...\n");

		while (!glfwWindowShouldClose(Render::window))
		{
			DeltaTime::UpdateDeltaTime();

			Engine::CalculateFPS();

			Render::WindowLoop();
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_LOOP,
			Type::INFO,
			"Exiting window loop...\n");
	}

    void Engine::CalculateFPS()
    {
        static high_resolution_clock::time_point previousFrameTime = high_resolution_clock::now();
        high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();

        auto frameDuration = duration_cast<microseconds>(currentFrameTime - previousFrameTime);
        fps = 1.0 / (frameDuration.count() / 1e6);

        previousFrameTime = currentFrameTime;

		Engine::CalculateDisplayedFPS();
    }

	void Engine::CalculateDisplayedFPS()
	{
		high_resolution_clock::time_point currentTime = high_resolution_clock::now();
		auto elapsedSeconds = duration_cast<duration<double>>(currentTime - lastUpdate).count();
		if (elapsedSeconds >= targetUpdateInterval)
		{
			lastUpdate = currentTime;
			displayedFPS = fps;
		}
	}
}