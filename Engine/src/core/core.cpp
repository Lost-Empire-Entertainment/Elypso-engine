//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <fstream>
#include <TlHelp32.h>
#include <tchar.h>

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
#include "gameobject.hpp"
#include "gui_settings.hpp"

using std::cout;
using std::endl;
using std::wstring;
using std::ifstream;
using std::filesystem::directory_iterator;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::current_path;
using std::filesystem::create_directory;

using Utils::String;
using Utils::File;
using Graphics::GUI::EngineGUI;
using Graphics::Render;
using EngineFile::SceneFile;
using EngineFile::ConfigFile;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Graphics::Shape::GameObjectManager;
using Graphics::GUI::GUISettings;

namespace Core
{
	void Engine::InitializeEngine()
	{
		if (IsThisProcessAlreadyRunning(name + ".exe"))
		{
			string title = "Already running";
			string message = "Error: '" + name + "' is already running!";

			CreateErrorPopup(title.c_str(), message.c_str());
		}

		cout << "\n==================================================\n"
			<< "\n"
			<< "ENTERED ELYPSO ENGINE\n"
			<< "\n"
			<< "==================================================\n"
			<< ".\n"
			<< ".\n"
			<< ".\n\n";

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

			docsPath = String::CharReplace(
				string(narrowPath.begin(), narrowPath.end()), '/', '\\') +
				"\\Elypso engine";

			if (!exists(docsPath)) File::CreateNewFolder(docsPath);
		}
		else
		{
			CreateErrorPopup("Path load error", "Couldn't find engine documents folder! Shutting down.");
		}

		//
		// SET FILES PATH
		//

		path fsFilesPath = current_path().generic_string() + "\\files";
		if (!exists(fsFilesPath))
		{
			CreateErrorPopup("Path load error", "Couldn't find files folder! Shutting down.");
			return;
		}
		filesPath = String::CharReplace(fsFilesPath.string(), '/', '\\');

		//
		// CHECK IF HUB PROJECT EXISTS OR NOT
		//

		ifstream projectFile(filesPath + "\\project.txt");
		if (!projectFile.is_open())
		{
			CreateErrorPopup("Project file load error", "Failed to open project file! Shutting down.");
		}

		//
		// SET SCENE AND PROJECT PATHS
		//

		//if engine is ran from repository structure
		string parentFolder = current_path().stem().string();
		if (parentFolder == "Release"
			|| parentFolder == "Debug")
		{
			gamePath = current_path()
				.parent_path()
				.parent_path()
				.parent_path()
				.generic_string() + "\\Game";
			gamePath = String::CharReplace(gamePath, '/', '\\');
			gameExePath = gamePath + "\\build\\Release\\" + GUISettings::gameName + ".exe";
			gameParentPath = gamePath + "\\build\\Release";
		}
		//if engine is not ran from repository structure
		else 
		{
			gamePath = current_path().parent_path().generic_string() + "\\Game";
			gamePath = String::CharReplace(gamePath, '/', '\\');
			gameExePath = gamePath + "\\build\\Release\\" + GUISettings::gameName + ".exe";
			gameParentPath = gamePath + "\\build\\Release";
		}

		//if neither one works then engine cannot proceed
		if (!exists(gamePath))
		{
			CreateErrorPopup("Game path load error", "Failed to find game template folder! Shutting down.");
		}

		string line;
		while (getline(projectFile, line))
		{
			if (!line.empty())
			{
				size_t pos_scene = line.find("scene:");
				if (pos_scene != string::npos)
				{
					string removable = "scene: ";
					size_t pos = line.find(removable);
					scenePath = line.erase(pos, removable.length());
				}

				size_t pos_project = line.find("project:");
				if (pos_project != string::npos)
				{
					string removable = "project: ";
					size_t pos = line.find(removable);
					projectPath = line.erase(pos, removable.length());
				}
			}
		}
		projectFile.close();

		//
		// SET SCENES AND TEXTURES PATHS
		//

		scenesPath = path(projectPath).parent_path().string() + "\\scenes";
		texturesPath = path(projectPath).parent_path().string() + "\\textures";

		//
		// REST OF THE INITIALIZATION
		//

		ConsoleManager::InitializeLogger();

		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::INFO,
			name + " " + version + "\n" +
			"Copyright (C) Lost Empire Entertainment 2024\n\n",
			true);

		ConfigFile::LoadConfigFile();

		SceneFile::CheckForProjectFile();

		string output = "Engine documents path: " + docsPath + "\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		output = "User engine files path: " + filesPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		Render::RenderSetup();

		//first scene is actually loaded when engine is ready for use
		SceneFile::LoadScene(scenePath);
	}

	void Engine::RunEngine()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::DEBUG,
			"Reached window loop successfully!\n\n");

		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::INFO,
			"==================================================\n\n",
			true,
			false);

		startedWindowLoop = true;
		if (!ConsoleManager::storedLogs.empty())
		{
			ConsoleManager::PrintLogsToBuffer();
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

		if (result == IDOK) Shutdown(true);
	}

	bool Engine::IsThisProcessAlreadyRunning(const string& processName)
	{
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32{};
		bool processFound = false;
		DWORD currentProcessId = GetCurrentProcessId();

		//take a snapshot of all processes
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			cout << "Error: CreateToolhelp32Snapshot failed!\n";
			return false;
		}

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32))
		{
			cout << "Error: Process32First failed!\n";
			CloseHandle(hProcessSnap);
			return false;
		}

		do
		{
			//compare the current process name with the one to check
			if (_stricmp(pe32.szExeFile, processName.c_str()) == 0)
			{
				//check if this is not the current process
				if (pe32.th32ProcessID != currentProcessId)
				{
					processFound = true;
					break;
				}
			}
		}
		while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return processFound;
	}

	void Engine::Shutdown(bool immediate)
	{
		if (immediate)
		{
			ConsoleManager::CloseLogger();
			EngineGUI::Shutdown();
			glfwTerminate();
			ExitProcess(1);
		}
		else
		{
			if (SceneFile::unsavedChanges == true)
			{
				//unminimize and bring to focus to ensure the user always sees the shutdown confirmation popup
				if (glfwGetWindowAttrib(Render::window, GLFW_ICONIFIED))
				{
					glfwRestoreWindow(Render::window);
				}
				glfwFocusWindow(Render::window);

				glfwSetWindowShouldClose(Render::window, GLFW_FALSE);
				EngineGUI::renderUnsavedShutdownWindow = true;
			}
			else
			{
				ConfigFile::SaveConfigFile();

				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::INFO,
					"==================================================\n\n",
					true);

				ConsoleManager::WriteConsoleMessage(
					Caller::SHUTDOWN,
					Type::INFO,
					"Cleaning up resources...\n");

				EngineGUI::Shutdown();

				//clean all glfw resources after program is closed
				glfwTerminate();

				ConsoleManager::WriteConsoleMessage(
					Caller::SHUTDOWN,
					Type::INFO,
					"Shutdown complete!\n");

				cout << "\n==================================================\n"
					<< "\n"
					<< "EXITED ELYPSO ENGINE\n"
					<< "\n"
					<< "==================================================\n"
					<< ".\n"
					<< ".\n"
					<< ".\n\n";

				ConsoleManager::CloseLogger();
			}
		}
	}
}