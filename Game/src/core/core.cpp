//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#define NOMINMAX
#include <Windows.h>
#include <ShlObj.h>
#include <fstream>
#include <filesystem>
#include <TlHelp32.h>

//game
#include "core.hpp"
#include "stringutils.hpp"
#include "fileutils.hpp"
#include "console.hpp"
#include "scenefile.hpp"
#include "configfile.hpp"
#include "render.hpp"
#include "gui.hpp"
#include "timemanager.hpp"

using std::cout;
using std::string;
using std::wstring;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::current_path;
using std::ifstream;
using std::filesystem::directory_iterator;

using Utils::String;
using Utils::File;
using GameFile::SceneFile;
using GameFile::ConfigFile;
using Graphics::Render;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Graphics::GUI::GameGUI;
using Core::TimeManager;

namespace Core
{
	void Game::Initialize()
	{
		for (const auto& entry : directory_iterator(current_path()))
		{
			string extension = path(entry).extension().string();
			if (extension == ".exe")
			{
				gameName = path(entry).stem().string();
				break;
			}
		}

		if (IsThisProcessAlreadyRunning(gameName + ".exe"))
		{
			CreateErrorPopup("Game is already running! Error code: F0010");
		}

		cout << "\n==================================================\n"
			<< "\n"
			<< "ENTERED GAME\n"
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
				"\\My Games\\" + gameName;

			if (!exists(docsPath)) File::CreateNewFolder(docsPath);
		}
		else
		{
			CreateErrorPopup("Couldn't find Game documents folder! Error code: F0011");
		}

		//
		// SET FILES PATH
		//

		string fsFilesPath = current_path().generic_string() + "\\files";
		if (!exists(fsFilesPath))
		{
			CreateErrorPopup("Couldn't find Game files folder! Error code: F0012");
			return;
		}
		filesPath = String::CharReplace(fsFilesPath, '/', '\\');

		//
		// FIND FIRST SCENE FILE
		//

		string firstSceneFile = docsPath + "\\firstScene.txt";
		if (!exists(firstSceneFile))
		{
			CreateErrorPopup("Couldn't find first scene file! Error code: F0013");
			return;
		}

		ifstream fscnFile(firstSceneFile);
		if (!fscnFile.is_open())
		{
			CreateErrorPopup("Couldn't read first scene file! Error code: F0014");
			return;
		}

		string project;
		string line;
		while (getline(fscnFile, line))
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

				if (type == "project")
				{
					project = value;
				}
				else if (type == "scene")
				{
					string parentPath = path(firstSceneFile).parent_path().string() + "\\" + project + "\\scenes";
					scenePath = parentPath + "\\" + value + "\\scene.txt";
				}
			}
		}
		fscnFile.close();

		if (scenePath == ""
			|| !exists(scenePath))
		{
			CreateErrorPopup("Couldn't find or open scene file! Error code: F0015");
			return;
		}

		//
		// SET SCENES AND TEXTURES PATHS
		//

		scenesPath = path(scenePath).parent_path().parent_path().string();
		if (!exists(scenesPath))
		{
			CreateErrorPopup("Couldn't find scenes folder! Error code: F0016");
			return;
		}

		texturesPath = path(docsPath).string() + "\\textures";
		if (!exists(scenesPath))
		{
			CreateErrorPopup("Couldn't find textures folder! Error code: F0017");
			return;
		}

		//
		// REST OF THE INITIALIZATION
		//

		ConsoleManager::InitializeLogger();

		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::INFO,
			"Initializing game...\n\n",
			true);

		ConfigFile::LoadConfigFile();

		string output = "Game documents path: " + docsPath + "\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		output = "Game files path: " + filesPath + "\n\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		Render::Initialize();

		//first scene is actually loaded when game is ready for use
		SceneFile::LoadScene(scenePath);
	}

	void Game::Run()
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

		isGameRunning = true;

		while (isGameRunning)
		{
			TimeManager::UpdateDeltaTime();
			Render::WindowLoop();

			// Check if the window should close (e.g., user closed the window)
			if (glfwWindowShouldClose(Render::window))
			{
				isGameRunning = false;
			}
		}
	}

	bool Game::IsThisProcessAlreadyRunning(const string& processName)
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
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return processFound;
	}

	void Game::CreateErrorPopup(const char* errorMessage)
	{
		string title = "Game has shut down";

		cout << "\n"
			<< "===================="
			<< "\n"
			<< "GAME SHUTDOWN"
			<< "\n\n"
			<< errorMessage
			<< "\n"
			<< "===================="
			<< "\n";

		int result = MessageBoxA(nullptr, errorMessage, title.c_str(), MB_ICONERROR | MB_OK);

		if (result == IDOK) Shutdown();
	}

	void Game::Shutdown()
	{
		isGameRunning = false;

		if (SceneFile::unsavedChanges == true)
		{
			/*
			//unminimize and bring to focus to ensure the user always sees the shutdown confirmation popup
			if (glfwGetWindowAttrib(Render::window, GLFW_ICONIFIED))
			{
				glfwRestoreWindow(Render::window);
			}
			glfwFocusWindow(Render::window);

			glfwSetWindowShouldClose(Render::window, GLFW_FALSE);
			GameGUI::renderUnsavedShutdownWindow = true;
			*/
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

			GameGUI::Shutdown();

			//clean all glfw resources after program is closed
			glfwTerminate();

			ConsoleManager::WriteConsoleMessage(
				Caller::SHUTDOWN,
				Type::INFO,
				"Shutdown complete!\n");

			cout << "\n==================================================\n"
				<< "\n"
				<< "EXITED GAME\n"
				<< "\n"
				<< "==================================================\n"
				<< ".\n"
				<< ".\n"
				<< ".\n\n";

			ConsoleManager::CloseLogger();
		}
	}
}