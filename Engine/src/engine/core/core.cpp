//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define NOMINMAX
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
#include "sceneFile.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "gameobject.hpp"
#if ENGINE_MODE
#include "gui_engine.hpp"
#include "gui_settings.hpp"
#else
#include "gui_game.hpp"
#endif

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

using Graphics::Render;
using EngineFile::SceneFile;
using EngineFile::ConfigFile;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Graphics::Shape::GameObjectManager;
#if ENGINE_MODE
using Graphics::GUI::EngineGUI;
using Graphics::GUI::GUISettings;
#else
using Graphics::GUI::GameGUI;
#endif

namespace Core
{
	void Engine::InitializeEngine(string assignedName, string assignedVersion)
	{
		name = assignedName;
		version = assignedVersion;

		string output;

		if (IsThisProcessAlreadyRunning(name + ".exe"))
		{
			CreateErrorPopup((name + " is already running! Error code: F0001").c_str());
		}

		cout << "\n==================================================\n"
			<< "\n"
#if ENGINE_MODE
			<< "ENTERED ENGINE\n"
#else
			<< "ENTERED GAME\n"
#endif
			<< "\n"
			<< "==================================================\n"
			<< ".\n"
			<< ".\n"
			<< ".\n\n";

		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::INFO,
			name + " " + version + "\n" +
			"Copyright (C) Lost Empire Entertainment 2024\n\n",
			true);

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

#if ENGINE_MODE
			docsPath = String::CharReplace(
				string(narrowPath.begin(), narrowPath.end()), '/', '\\') +
				"\\" + name;
#else
			string myGamesFolder = String::CharReplace(
				string(narrowPath.begin(), narrowPath.end()), '/', '\\') +
				"\\My Games";

			if (!exists(myGamesFolder))
			{
				File::CreateNewFolder(myGamesFolder);
			}

			docsPath = myGamesFolder + "\\" + name;
#endif

			if (!exists(docsPath)) File::CreateNewFolder(docsPath);

			output = "Documents path: " + docsPath + "\n";
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::DEBUG,
				output);
		}
		else
		{
			CreateErrorPopup(("Couldn't find " + name + " documents folder! Error code: F0002").c_str());
		}

#if ENGINE_MODE
		//
		// SET GAME PATHS
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
		//if engine is ran from visual studio folder
		else if (parentFolder == "x64-release"
				 || parentFolder == "x64-debug")
		{
			gamePath = current_path()
				.parent_path()
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

		output = "Game path: " + gamePath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		output = "Game exe path: " + gameExePath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		output = "Game parent path: " + gameParentPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		//if neither one works then engine cannot proceed
		if (!exists(gamePath))
		{
			CreateErrorPopup("Failed to find game template folder! Error code: F0003");
		}
#endif
		//
		// SET FILES PATH
		//

		filesPath = current_path().generic_string() + "\\files";
		if (!exists(filesPath))
		{
			CreateErrorPopup("Couldn't find files folder! Error code: F0004");
			return;
		}

		output = name + " files path: " + filesPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		//
		// SET PROJECT FOLDER PATH
		//

		ifstream projectFile(docsPath + "\\project.txt");
		if (!projectFile.is_open())
		{
			CreateErrorPopup("Failed to open project file! Error code: F0005");
		}

		output = "Project file path: " + docsPath + "\\project.txt" + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		string line;
		while (getline(projectFile, line))
		{
			if (!line.empty())
			{
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

		output = "Project path: " + projectPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		//
		// SET SCENES AND TEXTURES PATHS
		//

		scenesPath = path(projectPath).string() + "\\scenes";

		output = "Scenes path: " + scenesPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		texturesPath = path(projectPath).string() + "\\textures";

		output = "Textures path: " + texturesPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		//
		// SET FIRST SCENE PATH
		//

		string firstSceneFile = docsPath + "\\" + path(docsPath).stem().string() + "\\firstScene.txt";
		if (exists(firstSceneFile))
		{
			ifstream fsFile(firstSceneFile);
			if (!fsFile.is_open())
			{
				CreateErrorPopup("Failed to open first scene file! Error code: F0006");
			}

			string line;
			while (getline(fsFile, line))
			{
				if (!line.empty()
					&& line.find("=") != string::npos)
				{
					vector<string> splitLine = String::Split(line, '=');
					string type = splitLine[0];
					string value = splitLine[1];

					//remove one space in front of value if it exists
					if (value[0] == ' ') value.erase(0, 1);
					//remove one space in front of each value comma if it exists
					for (size_t i = 0; i < value.length(); i++)
					{
						if (value[i] == ','
							&& i + 1 < value.length()
							&& value[i + 1] == ' ')
						{
							value.erase(i + 1, 1);
						}
					}

					if (type == "scene")
					{
						string parentPath = 
							path(firstSceneFile).parent_path().string() + "\\" 
							+ path(projectPath).stem().string() + "\\scenes";

						gameFirstScene = parentPath + "\\" + value + "\\scene.txt";
					}
				}
			}
			fsFile.close();

			bool foundExisting = false;
			for (const auto& entry : directory_iterator(scenesPath))
			{
				string entryStem = path(entry).stem().string();
				if (gameFirstScene == entryStem)
				{
					foundExisting = true;
					break;
				}
			}

			if (!foundExisting)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Failed to assign valid first scene file because the provided one does not exist! Clearing assigned value.");
				gameFirstScene = "";
			}
			else
			{
				output = "Game first scene path: " + gameFirstScene + "\n\n";
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::DEBUG,
					output);
			}
		}

		//
		// REST OF THE INITIALIZATION
		//

		ConsoleManager::InitializeLogger();

		ConfigFile::LoadConfigFile();

		Render::RenderSetup();

		string lastSavedScenePath = Engine::docsPath + "\\lastSavedScene.txt";
		if (exists(lastSavedScenePath))
		{
			ifstream lastSavedSceneFile(lastSavedScenePath);
			if (!lastSavedSceneFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Couldn't open scene file '" + lastSavedScenePath + "'! Opening default scene.\n");
				SceneFile::LoadScene(scenesPath + "\\Scene1\\scene.txt");
			}
			else 
			{
				string line;
				string foundScenePath;
				while (getline(lastSavedSceneFile, line))
				{
					foundScenePath = line;
					break;
				}
				lastSavedSceneFile.close();

				if (exists(foundScenePath))
				{
					SceneFile::LoadScene(scenesPath + "\\Scene1\\scene.txt");
				}
				else 
				{
					if (foundScenePath != "")
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::FILE,
							Type::EXCEPTION,
							"Couldn't load scene file '" + foundScenePath + "' because it doesn't exist! Opening default scene.\n");
					}
					SceneFile::LoadScene(scenesPath + "\\Scene1\\scene.txt");
				}
			}
		}
		//otherwise load first scene
		else SceneFile::LoadScene(scenesPath + "\\Scene1\\scene.txt");
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
#if ENGINE_MODE
		if (!ConsoleManager::storedLogs.empty())
		{
			ConsoleManager::PrintLogsToBuffer();
		}
#endif

		isEngineRunning = true;

		while (isEngineRunning)
		{
			TimeManager::UpdateDeltaTime();
			Render::WindowLoop();

			// Check if the window should close (e.g., user closed the window)
			if (glfwWindowShouldClose(Render::window))
			{
				isEngineRunning = false;
			}
		}
	}

	void Engine::CreateErrorPopup(const char* errorMessage)
	{
		string title = name + " has shut down";

		cout << "\n"
			<< "===================="
			<< "\n"
#if ENGINE_MODE
			<< "ENGINE SHUTDOWN\n"
#else
			<< "GAME SHUTDOWN\n"
#endif
			<< "\n\n"
			<< errorMessage 
			<< "\n"
			<< "===================="
			<< "\n";

		int result = MessageBoxA(nullptr, errorMessage, title.c_str(), MB_ICONERROR | MB_OK);

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
			isEngineRunning = false;

			ConsoleManager::CloseLogger();
#if ENGINE_MODE
			EngineGUI::Shutdown();
#else
			GameGUI::Shutdown();
#endif
			glfwTerminate();
		}
		else
		{
			if (SceneFile::unsavedChanges == true)
			{
#if ENGINE_MODE
				//unminimize and bring to focus to ensure the user always sees the shutdown confirmation popup
				if (glfwGetWindowAttrib(Render::window, GLFW_ICONIFIED))
				{
					glfwRestoreWindow(Render::window);
				}
				glfwFocusWindow(Render::window);

				glfwSetWindowShouldClose(Render::window, GLFW_FALSE);
				EngineGUI::renderUnsavedShutdownWindow = true;
#else
				SceneFile::unsavedChanges = false;
				Shutdown();
#endif
			}
			else
			{
				isEngineRunning = false;

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

#if ENGINE_MODE
				EngineGUI::Shutdown();
#else
				GameGUI::Shutdown();
#endif

				//clean all glfw resources after program is closed
				glfwTerminate();

				ConsoleManager::WriteConsoleMessage(
					Caller::SHUTDOWN,
					Type::INFO,
					"Shutdown complete!\n");

				cout << "\n==================================================\n"
					<< "\n"
#if ENGINE_MODE
					<< "EXITED ENGINE\n"
#else
					<< "EXITED GAME\n"
#endif
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