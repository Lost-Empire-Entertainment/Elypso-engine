//This script is a part of the Game Template that is distributed by Elypso Engine.
//If you have access to this script then you have full freedom to do anything with it.
//A copy of the MIT license has been included in LICENSE.md

#include <iostream>
#include <Windows.h>
#include <ShlObj.h>
#include <fstream>

#include <filesystem>

//game
#include "core.hpp"
#include "stringutils.hpp"
#include "fileutils.hpp"
#include "console.hpp"
#include "scenefile.hpp"
#include "configfile.hpp"
#include "render.hpp"

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

		Console::InitializeLogger();

		Console::WriteConsoleMessage(
			Caller::INPUT,
			Type::INFO,
			gameName + " " + gameVersion + "\n" +
			"Initializing game...\n\n",
			true);

		ConfigFile::LoadConfigFile();

		SceneFile::CheckForProjectFile();

		string output = "Game documents path: " + docsPath + "\n";
		Console::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		output = "User engine files path: " + filesPath + "\n\n";
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

	void Game::Shutdown()
	{
		cout << "end\n";
	}

	bool Game::IsThisProcessAlreadyRunning(const string& processName)
	{
		return false;
	}

	void Game::CreateErrorPopup(const string& title, const string& description)
	{

	}
}