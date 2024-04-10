//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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
#include <ShlObj.h>
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
#include "stringUtils.hpp"

using std::cout;
using std::endl;
using std::wstring;
using std::filesystem::directory_iterator;
using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::remove_all;
using std::filesystem::is_regular_file;
using std::filesystem::is_directory;
using std::filesystem::path;
using std::filesystem::current_path;
using std::filesystem::create_directory;
using std::filesystem::copy_file;

using Utils::String;
using Utils::File;
using Graphics::GUI::EngineGUI;
using Graphics::Render;
using EngineFile::SceneFile;
using EngineFile::ConfigFileManager;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void Engine::InitializeEngine()
	{
		//
		// SET DOCUMENTS PATH
		//

		PWSTR docsFolderWidePath;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &docsFolderWidePath);
		if (SUCCEEDED(result))
		{
			wstring wPath(docsFolderWidePath);
			CoTaskMemFree(docsFolderWidePath); //free the allocated memory

			//get the required buffer size
			int size_needed = WideCharToMultiByte(
				CP_UTF8,
				0,
				wPath.c_str(),
				static_cast<int>(wPath.length()),
				NULL,
				0,
				NULL,
				NULL);

			//convert wide string to utf-8 encoded narrow string
			string narrowPath(size_needed, 0);
			WideCharToMultiByte(
				CP_UTF8,
				0,
				wPath.c_str(),
				static_cast<int>(wPath.length()),
				&narrowPath[0],
				size_needed,
				NULL,
				NULL);

			docsPath = String::StringReplace(
				string(narrowPath.begin(), narrowPath.end()), "\\", "/") +
				"/" + "Elypso engine";

			if (!exists(docsPath)) create_directory(docsPath);
		}
		else
		{
			Engine::CreateErrorPopup("Path load error", "Couldn't find engine documents folder! Shutting down.");
		}

		//
		// SET FILES PATH
		//

		path fsFilesPath = current_path().generic_string() + "/files/engine";
		if (!exists(fsFilesPath))
		{
			Engine::CreateErrorPopup("Path load error", "Couldn't find files folder! Shutting down.");
			return;
		}
		filesPath = fsFilesPath.string();

		ConfigFileManager::LoadConfigFile();

		SceneFile::CheckForProjectFile();

		Logger::InitializeLogger();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			Engine::name + " " + Engine::version + "\n" +
			"Copyright (C) Lost Empire Entertainment 2024\n\n",
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

		Render::RenderSetup();

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
			ConfigFileManager::SaveConfigFile();

			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"==================================================\n\n",
				true);

			ConsoleManager::WriteConsoleMessage(
				Caller::SHUTDOWN,
				Type::INFO,
				"Cleaning up resources...\n");

			//todo: copy scene files to external project scenes folder
			//todo: fix crash when loading scene with pointlight or spotlight

			string files = current_path().generic_string() + "/files/project";
			for (const auto& entry : directory_iterator(files))
			{
				path entryPath(entry);
				if (is_directory(entryPath)) remove_all(entryPath);
				else if (is_regular_file(entryPath)) remove(entryPath);
			}

			EngineGUI::Shutdown();

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