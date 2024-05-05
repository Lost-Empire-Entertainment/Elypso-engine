//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <fstream>

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
using EngineFile::ConfigFileManager;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Graphics::Shape::GameObjectManager;

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

		path fsFilesPath = current_path().generic_string() + "/files";
		if (!exists(fsFilesPath))
		{
			Engine::CreateErrorPopup("Path load error", "Couldn't find files folder! Shutting down.");
			return;
		}
		filesPath = fsFilesPath.string();

		//
		// SET PROJECT PATH
		//

		ifstream projectFile(filesPath + "/project.txt");
		if (!projectFile.is_open())
		{
			Engine::CreateErrorPopup("Project file load error", "Failed to open project file! Shutting down engine");
		}

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

		//
		// FILL GAMEOBJECT CATEGORY AND TYPE VECTORS
		//

		map<string, vector<string>> gameObjects;

		vector<string> cat_characters;
		cat_characters.push_back("Placeholder");
		gameObjects.insert({ "Characters", cat_characters });

		vector<string> cat_effects;
		cat_effects.push_back("Placeholder");
		gameObjects.insert({ "Effects", cat_effects });

		vector<string> cat_audio;
		cat_audio.push_back("Placeholder");
		gameObjects.insert({ "Audio", cat_audio });

		vector<string> cat_UI;
		cat_UI.push_back("Placeholder");
		gameObjects.insert({ "UI", cat_UI });

		vector<string> cat_lights;
		cat_lights.push_back("Spotlights");
		cat_lights.push_back("Point lights");
		gameObjects.insert({ "Lights", cat_lights });

		vector<string> cat_textures;
		cat_textures.push_back("Diffuse textures");
		cat_textures.push_back("Specular textures");
		cat_textures.push_back("Normal textures");
		cat_textures.push_back("Height textures");
		gameObjects.insert({ "Textures", cat_textures });

		vector<string> cat_props;
		cat_props.push_back("Static props");
		gameObjects.insert({ "Props", cat_props });

		GameObjectManager::SetCategoryNames(gameObjects);

		//
		// REST OF THE INITIALIZATION
		//

		ConsoleManager::InitializeLogger();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			Engine::name + " " + Engine::version + "\n" +
			"Copyright (C) Lost Empire Entertainment 2024\n\n",
			true);

		ConfigFileManager::LoadConfigFile();

		SceneFile::CheckForProjectFile();

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

			File::DeleteFileOrfolder(Engine::filesPath + "/project.txt");
			cout << "deleted " << Engine::filesPath + "/project.txt\n";

			EngineGUI::Shutdown();

			//clean all glfw resources after program is closed
			glfwTerminate();

			ConsoleManager::WriteConsoleMessage(
				Caller::SHUTDOWN,
				Type::INFO,
				"Shutdown complete!\n");

			ConsoleManager::CloseLogger();
		}
	}
}