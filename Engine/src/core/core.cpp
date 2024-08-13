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
			CreateErrorPopup("Elypso Engine is already running! Error code: F0001");
		}

		cout << "\n==================================================\n"
			<< "\n"
			<< "ENTERED ELYPSO ENGINE\n"
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

			docsPath = String::CharReplace(
				string(narrowPath.begin(), narrowPath.end()), '/', '\\') +
				"\\Elypso engine";

			if (!exists(docsPath)) File::CreateNewFolder(docsPath);

			string output = "Engine documents path: " + docsPath + "\n";
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::DEBUG,
				output);
		}
		else
		{
			CreateErrorPopup(
				"Couldn't find engine documents folder! Error code: F0002");
		}

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
		//if engine is not ran from repository structure
		else 
		{
			gamePath = current_path().parent_path().generic_string() + "\\Game";
			gamePath = String::CharReplace(gamePath, '/', '\\');
			gameExePath = gamePath + "\\build\\Release\\" + GUISettings::gameName + ".exe";
			gameParentPath = gamePath + "\\build\\Release";
		}

		string output = "Game path: " + gamePath + "\n\n";
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
			CreateErrorPopup(
				"Failed to find game template folder! Error code: F0005");
		}

		//
		// SET FILES PATH
		//

		filesPath = current_path().generic_string() + "\\files";
		if (!exists(filesPath))
		{
			CreateErrorPopup(
				"Couldn't find files folder! Error code: F0003");
			return;
		}

		output = "User engine files path: " + filesPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		//
		// SET PROJECT FOLDER PATH
		//

		ifstream projectFile(filesPath + "\\project.txt");
		if (!projectFile.is_open())
		{
			CreateErrorPopup(
				"Failed to open project file! Error code: F0004");
		}

		output = "Project file path: " + filesPath + "\\project.txt" + "\n\n";
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

		string firstSceneFile = docsPath + "\\" + path(gameExePath).stem().string() + "\\firstScene.txt";
		if (exists(firstSceneFile))
		{
			ifstream fsFile(firstSceneFile);
			if (!fsFile.is_open())
			{
				CreateErrorPopup(
					"Failed to open first scene file! Error code: F0006");
			}

			string line;
			while (getline(fsFile, line))
			{
				gameFirstScene = line;
				break;
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
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Couldn't load scene file '" + foundScenePath + "' because it doesn't exist! Opening default scene.\n");
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

	void Engine::CreateErrorPopup(const char* errorMessage)
	{
		string title = "Elypso Engine has shut down";

		cout << "\n"
			<< "===================="
			<< "\n"
			<< "ENGINE SHUTDOWN"
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