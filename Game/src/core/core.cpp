//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <TlHelp32.h>

//game
#include "core.hpp"
#include "render.hpp"
#include "timeManager.hpp"
#include "configFile.hpp"
#include "sceneFile.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "model.hpp"
#include "gameobject.hpp"
#include "gui.hpp"

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
using Graphics::Render;
using GameFile::SceneFile;
using GameFile::ConfigFileManager;
using Graphics::Shape::Model;
using Graphics::Shape::GameObjectManager;
using Graphics::GUI::GameGUI;

namespace Core
{
	void Game::InitializeGame()
	{
		if (IsThisProcessAlreadyRunning(name + ".exe"))
		{
			string title = "Engine already running";
			string message = "Error: '" + name + "' is already running!";

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

			docsPath = String::StringReplace(string(narrowPath.begin(), narrowPath.end()), "\\", "/") + "/Game";

			if (!exists(docsPath)) create_directory(docsPath);
		}
		else
		{
			Game::CreateErrorPopup("Path load error", "Couldn't find game documents folder! Shutting down.");
		}

		//
		// SET FILES PATH
		//

		path fsFilesPath = current_path().generic_string() + "/files/game";
		if (!exists(fsFilesPath))
		{
			Game::CreateErrorPopup("Path load error", "Couldn't find files folder! Shutting down.");
			return;
		}
		filesPath = fsFilesPath.string();

		ConfigFileManager::LoadConfigFile();

		SceneFile::CheckForGameFile();

		cout << "Game documents path: " << Game::docsPath << "\n";

		cout << "User game files path: " << Game::filesPath << "\n\n";

		Render::RenderSetup();

		//first scene is actually loaded when game is ready for use
		SceneFile::LoadScene(SceneFile::currentScenePath);
	}

	void Game::RunGame()
	{
		cout << "Reached window loop successfully!\n\n";

		cout << "==================================================\n\n";

		startedWindowLoop = true;

		while (!glfwWindowShouldClose(Render::window))
		{
			TimeManager::UpdateDeltaTime();

			Render::WindowLoop();
		}
	}

	void Game::CreateErrorPopup(const char* errorTitle, const char* errorMessage)
	{
		int result = MessageBoxA(nullptr, errorMessage, errorTitle, MB_ICONERROR | MB_OK);

		if (result == IDOK) Shutdown();
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

	void Game::Shutdown(bool immediate)
	{
		if (immediate)
		{
			GameGUI::Shutdown();
			glfwTerminate();
			ExitProcess(1);
		}
		else
		{
			if (SceneFile::unsavedChanges == true)
			{
				glfwSetWindowShouldClose(Render::window, GLFW_FALSE);
			}
			else
			{
				ConfigFileManager::SaveConfigFile();

				cout << "==================================================\n\n";

				cout << "Cleaning up resources...\n";

				GameGUI::Shutdown();

				//clean all glfw resources after program is closed
				glfwTerminate();

				cout << "\n==================================================\n"
					<< "\n"
					<< "EXITED GAME\n"
					<< "\n"
					<< "==================================================\n"
					<< ".\n"
					<< ".\n"
					<< ".\n\n";
			}
		}
	}
}