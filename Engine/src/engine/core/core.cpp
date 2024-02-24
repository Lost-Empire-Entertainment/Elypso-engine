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

#include <Windows.h>
#include <filesystem>

//engine
#include "core.hpp"
#include "console.hpp"
#include "render.hpp"
#include "timeManager.hpp"
#include "configFile.hpp"
#include "gui.hpp"
#include "sceneFile.hpp"
#include "fileUtils.hpp"

using std::cout;
using std::endl;
using std::filesystem::directory_iterator;
using std::filesystem::current_path;
using std::filesystem::remove;
using std::filesystem::remove_all;
using std::filesystem::is_regular_file;
using std::filesystem::is_directory;

using Utils::File;
using Graphics::GUI::EngineGUI;
using Graphics::Render;
using EngineFile::SceneFile;
using EngineFile::ConfigFile;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void Engine::InitializeEngine()
	{
		SceneFile::CheckForProjectFile();

		ConfigFile::ProcessFirstConfigValues();

		Logger::InitializeLogger();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			Engine::name + " " + Engine::version + "\n" +
			"Copyright (C) Greenlaser 2024\n\n",
			true);

		string output = "Engine documents path: " + Engine::docsPath + "\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::DEBUG,
			output);

		output = "User engine files path: " + Engine::filesPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::DEBUG,
			output);

		output = "Engine files path: " + Engine::enginePath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::DEBUG,
			output);

		Render::RenderSetup();

		ConfigFile::ProcessConfigFile("config.txt");

		//first scene is actually loaded when engine is ready for use
		SceneFile::LoadScene(SceneFile::currentScenePath);
	}

	void Engine::RunEngine()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_LOOP,
			Type::DEBUG,
			"Reached window loop successfully!\n\n");

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"==================================================\n\n",
			true,
			false);

		startedWindowLoop = true;
		if (!Core::ConsoleManager::storedLogs.empty()) 
		{
			Core::ConsoleManager::PrintLogsToBuffer();
		}

		while (!glfwWindowShouldClose(Render::window))
		{
			TimeManager::UpdateDeltaTime();

			Render::WindowLoop();
		}
	}

	void Engine::CreateErrorPopup(const char* errorTitle, const char* errorMessage)
	{
		int result = MessageBoxA(nullptr, errorMessage, errorTitle, MB_ICONERROR | MB_OK);

		if (result == IDOK) Shutdown();
	}

	void Engine::Shutdown()
	{
		if (SceneFile::unsavedChanges == true)
		{
			glfwSetWindowShouldClose(Render::window, GLFW_FALSE);
			EngineGUI::renderUnsavedShutdownWindow = true;
		}
		else
		{
			ConfigFile::SaveDataAtShutdown();

			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"==================================================\n\n",
				true);

			ConsoleManager::WriteConsoleMessage(
				Caller::SHUTDOWN,
				Type::INFO,
				"Cleaning up resources...\n");

			string files = current_path().generic_string() + "/files";
			for (const auto& entry : directory_iterator(files))
			{
				path entryPath(entry);
				if (is_directory(entryPath)) remove_all(entryPath);
				else if (is_regular_file(entryPath)) remove(entryPath);
			}

			EngineGUI::GetInstance().Shutdown();

			//clean all glfw resources after program is closed
			glfwTerminate();

			ConsoleManager::WriteConsoleMessage(
				Caller::SHUTDOWN,
				Type::INFO,
				"Shutdown complete!\n");

			Logger::CloseLogger();
		}
	}
}