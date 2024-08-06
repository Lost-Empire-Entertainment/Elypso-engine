//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
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

using std::cout;
using std::string;
using std::wstring;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::current_path;
using std::ifstream;

using Utils::String;
using Utils::File;
using GameFile::SceneFile;
using GameFile::ConfigFile;
using Graphics::Render;
using Caller = Core::Console::Caller;
using Type = Core::Console::Type;
using Graphics::GUI::GameGUI;

namespace Core
{
	void Game::Initialize()
	{
		cout << "start\n";

		if (IsThisProcessAlreadyRunning(gameName + ".exe"))
		{
			string title = "Already running";
			string message = "Error: '" + gameName + "' is already running!";

			CreateErrorPopup(title.c_str(), message.c_str());
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
				"\\" + gameName;

			if (!exists(docsPath)) File::CreateNewFolder(docsPath);
		}
		else
		{
			CreateErrorPopup(
				"Path load error", 
				"Couldn't find engine documents folder! Shutting down.");
		}

		//
		// SET INTERNAL AND EXTERNAL FILE PATHS
		//

		path fsFilesPath = current_path().generic_string() + "\\files\\game";
		if (!exists(fsFilesPath))
		{
			CreateErrorPopup(
				"Path load error", 
				"Couldn't find game files folder! Shutting down.");
			return;
		}
		filesPath = String::CharReplace(fsFilesPath.string(), '/', '\\');

		path fsExtFilesPath = current_path().generic_string() + "\\files\\project";
		if (!exists(fsExtFilesPath))
		{
			CreateErrorPopup(
				"Path load error",
				"Couldn't find project files folder! Shutting down.");
			return;
		}
		externalFilesPath = String::CharReplace(fsExtFilesPath.string(), '/', '\\');

		//
		// FIND FIRST SCENE FILE
		//

		string firstSceneFile = externalFilesPath + "\\firstscene.txt";
		if (!exists(firstSceneFile))
		{
			CreateErrorPopup(
				"Path load error",
				"Couldn't find first scene file! Shutting down.");
			return;
		}

		ifstream fscnFile(firstSceneFile);
		if (!fscnFile.is_open())
		{
			CreateErrorPopup(
				"File read error",
				"Couldn't read first scene file! Shutting down.");
			return;
		}

		string line;
		while (getline(fscnFile, line))
		{
			if (!line.empty())
			{
				scenePath = line;
				break;
			}
		}
		if (scenePath == ""
			|| !exists(scenePath))
		{
			CreateErrorPopup(
				"Path load error",
				"Couldn't find or open scene file! Shutting down.");
			return;
		}

		//
		// SET SCENES AND TEXTURES PATHS
		//

		scenesPath = path(externalFilesPath).string() + "\\scenes";
		if (!exists(scenesPath))
		{
			CreateErrorPopup(
				"Path load error",
				"Couldn't find scenes folder! Shutting down.");
			return;
		}
		string output = "Game scenes path: " + scenesPath + "\n";
		cout << output << "\n";

		texturesPath = path(externalFilesPath).string() + "\\textures";
		if (!exists(scenesPath))
		{
			CreateErrorPopup(
				"Path load error",
				"Couldn't find textures folder! Shutting down.");
			return;
		}
		output = "Game textures path: " + texturesPath + "\n";
		cout << output << "\n";

		//
		// REST OF THE INITIALIZATION
		//

		Console::InitializeLogger();

		Console::WriteConsoleMessage(
			Caller::INPUT,
			Type::INFO,
			gameName + " " + gameVersion + "\n" +
			"Initializing game...\n\n",
			true);

		ConfigFile::LoadConfigFile();

		SceneFile::CheckForProjectFile();

		output = "Game documents path: " + docsPath + "\n";
		cout << output << "\n";
		Console::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		output = "Game files path: " + filesPath + "\n\n";
		cout << output << "\n";
		Console::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		Render::Initialize();

		//first scene is actually loaded when engine is ready for use
		SceneFile::LoadScene(scenePath);
	}

	void Game::Run()
	{
		cout << "run\n";
	}

	void Game::Shutdown(bool immediate)
	{
		if (immediate)
		{
			Console::CloseLogger();
			GameGUI::Shutdown();
			//glfwTerminate();
			ExitProcess(1);
		}
		else
		{
			if (SceneFile::unsavedChanges == true)
			{
				//unminimize and bring to focus to ensure the user always sees the shutdown confirmation popup
				/*
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

				Console::WriteConsoleMessage(
					Caller::INPUT,
					Type::INFO,
					"==================================================\n\n",
					true);

				Console::WriteConsoleMessage(
					Caller::SHUTDOWN,
					Type::INFO,
					"Cleaning up resources...\n");

				GameGUI::Shutdown();

				//clean all glfw resources after program is closed
				//glfwTerminate();

				Console::WriteConsoleMessage(
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

				Console::CloseLogger();
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

	void Game::CreateErrorPopup(const char* errorTitle, const char* errorMessage)
	{
		int result = MessageBoxA(nullptr, errorMessage, errorTitle, MB_ICONERROR | MB_OK);

		if (result == IDOK) Shutdown(true);
	}
}