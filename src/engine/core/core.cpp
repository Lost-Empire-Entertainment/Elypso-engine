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
#include "core.hpp"
#include "admin.hpp"
#include "console.hpp"
#include "render.hpp"
#include "shutdown.hpp"
#include "timeManager.hpp"

#include <string>

using namespace Graphics;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void Engine::InitializeEngine()
	{
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
			TimeManager::UpdateDeltaTime();

			Render::WindowLoop();
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_LOOP,
			Type::INFO,
			"Exiting window loop...\n");
	}
}