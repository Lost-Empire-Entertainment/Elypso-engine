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
#include "glad.h"
#include "glfw3.h"

//engine
#include "console.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "shutdown.hpp"
#include "configFile.hpp"
#include "fileUtils.hpp"

#include <filesystem>

using std::filesystem::directory_iterator;
using std::filesystem::current_path;
using std::filesystem::remove;
using std::filesystem::remove_all;
using std::filesystem::is_regular_file;
using std::filesystem::is_directory;

using Utils::File;
using Graphics::GUI::EngineGUI;
using Graphics::Render;
using EngineFile::ConfigFile;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void ShutdownManager::Shutdown()
	{
		shouldShutDown = true;

		ConfigFile::SaveDataAtShutdown();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"==================================================\n\n",
			true,
			false);

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
			Type::SUCCESS,
			"Shutdown complete!\n");

		Logger::CloseLogger();
	}
}