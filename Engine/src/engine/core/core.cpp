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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//engine
#include "console.hpp"
#include "core.hpp"
#include "render.hpp"
#include "shutdown.hpp"
#include "timeManager.hpp"
#include "configFile.hpp"
#include "gui.hpp"

using std::cout;
using std::endl;

using Core::ShutdownManager;
using File::ConfigFile;
using Graphics::Render;
using Graphics::GUI;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void Engine::InitializeEngine()
	{
		ConfigFile::ProcessFirstConfigValues();

		Logger::InitializeLogger();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Elypso engine " + Engine::version + "\n" +
			"Copyright (C) Greenlaser 2023\n\n",
			true);

		cout << "Game documents path: " << Engine::docsPath << endl;
		cout << "Game files path: " << Engine::filesPath << "\n\n";

		Render::RenderSetup();

		ConfigFile::ProcessConfigFile("config.txt");
	}

	void Engine::RunEngine()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_LOOP,
			Type::SUCCESS,
			"Reached window loop successfully!\n");

		startedWindowLoop = true;
		if (!Core::ConsoleManager::storedLogs.empty()) 
		{
			Core::ConsoleManager::PrintLogsToBuffer();
		}

		while (!glfwWindowShouldClose(Render::window)
			   && !ShutdownManager::shouldShutDown)
		{
			TimeManager::UpdateDeltaTime();

			Render::WindowLoop();
		}

		ConfigFile::SaveDataAtShutdown();

		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_LOOP,
			Type::DEBUG,
			"Exiting window loop...\n");
	}
}