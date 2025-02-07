//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <iostream>
#include <filesystem>
#include <thread>
#include <fstream>
#include <cstdio>
#include <memory>
#include <array>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glfw3.h"

//engine
#include "compile.hpp"
#include "console.hpp"
#include "core.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "gameobject.hpp"
#include "render.hpp"
#include "sceneFile.hpp"
#include "gui_engine.hpp"
#include "gui_settings.hpp"
#include "configFile.hpp"

using std::cout;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::exception;
using std::filesystem::exists;
using std::thread;
using std::ofstream;
using std::runtime_error;
using std::array;
using std::unique_ptr;
using std::filesystem::current_path;
using std::filesystem::is_directory;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using Utils::String;
using Utils::File;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Render;
using EngineFile::SceneFile;
using Graphics::GUI::EngineGUI;
using Graphics::GUI::GUISettings;
using EngineFile::ConfigFile;

namespace Core
{
	void Compilation::Compile()
	{
		output.clear();

		renderBuildingWindow = true;
		finishedBuild = false;

		if (SceneFile::unsavedChanges) SceneFile::SaveScene();

		thread CompileThread([]()
			{
				//
				// REMOVE OLD GAME EXE IF ANY EXISTS
				//

				if (exists(Engine::gameParentPath))
				{
					for (const auto& file : directory_iterator(Engine::gameParentPath))
					{
						string filePath = path(file).string();
						string fileExtension = path(file).extension().string();

						if (fileExtension == ".exe")
						{
							File::DeleteFileOrfolder(filePath);
						}
					}
				}

				//
				// START BUILDING GAME FROM SOURCE CODE
				//
				if (RunInstaller())
				{
					string gameStem = path(Engine::gameExePath).stem().string();
					if (gameStem != "Game")
					{
						File::MoveOrRenameFileOrFolder(
							path(Engine::gameParentPath) / "Game.exe",
							Engine::gameExePath,
							true);
					}

					//
					// COPY PROJECT FILE TO GAME DOCUMETS FOLDER
					//

					string targetFolder = (path(Engine::gameParentPath) / "project").string();
					if (exists(targetFolder)) File::DeleteFileOrfolder(targetFolder);
					File::CreateNewFolder(targetFolder);

					string projectFileOriginPath = (path(Engine::docsPath) / "project.txt").string();
					string projectFileTargetPath = (path(targetFolder) / "project.txt").string();
					if (exists(projectFileTargetPath)) File::DeleteFileOrfolder(projectFileTargetPath);
					File::CopyFileOrFolder(projectFileOriginPath, projectFileTargetPath);

					//
					// COPY SCENE FILES TO GAME EXE FOLDER
					//

					string scenePath = path(Engine::projectPath).string();

					for (const auto& entry : directory_iterator(path(scenePath)))
					{
						string stem = path(entry).stem().string();

						if (is_directory(entry))
						{
							string origin = path(entry).string();
							string originFileName = path(entry).filename().string();
							string target = (path(targetFolder) / originFileName).string();

							File::CopyFileOrFolder(origin, target);
						}
					}

					//
					// CREATE FIRST SCENE FILE WHICH GAME LOADS FROM WHEN GAME EXE IS RAN
					//

					string firstSceneFilePath = (path(targetFolder) / "firstScene.txt").string();
					if (exists(firstSceneFilePath)) File::DeleteFileOrfolder(firstSceneFilePath);

					ofstream firstSceneFile(firstSceneFilePath);
					if (!firstSceneFile.is_open())
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::FILE,
							Type::EXCEPTION,
							"Error: Compilation failed because first scene file couldnt be created!\n");

						renderBuildingWindow = false;

						return;
					}

					firstSceneFile << "project= " << path(Engine::projectPath).stem().string() << "\n";
					firstSceneFile << "scene= " << Engine::gameFirstScene;

					firstSceneFile.close();

					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::INFO,
						"Compilation succeeded!\n");

					finishedBuild = true;
				}
				finishedBuild = true;
			});

		CompileThread.detach();
	}
	
	bool Compilation::RunInstaller()
	{
		string engineRootFolder = "";
		string gameRootFolder = "";

		string parentFolder = current_path().stem().string();
		//if engine is ran from repository or visual studio folder
		string msvcRelease = "x64-release";
		string msvcDebug = "x64-debug";

		if (parentFolder == msvcRelease
			|| parentFolder == msvcDebug)
		{
			engineRootFolder = (current_path().parent_path().parent_path().parent_path()).string();
			gameRootFolder = (current_path().parent_path().parent_path().parent_path().parent_path() / "Game").string();
		}
		else if (parentFolder == "Engine")
		{
			engineRootFolder = current_path().string();
			gameRootFolder = (current_path().parent_path() / "Game").string();
		}

		if (gameRootFolder == ""
			|| !exists(gameRootFolder))
		{
			string output = "Game root folder '" + gameRootFolder + "' is empty or does not exist!\n";
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				output);
			return false;
		}

		string originLib{};
		string targetLib{};
		string releaseType{};

#ifdef _WIN32

#ifdef NDEBUG
		releaseType = "release";
		originLib = (path(engineRootFolder) / "Elypso engine.lib").string();
		targetLib = (path(gameRootFolder) / "Elypso engine.lib").string();
#else
		releaseType = "debug";
		originLib = (path(engineRootFolder) / "Elypso engineD.lib").string();
		targetLib = (path(gameRootFolder) / "Elypso engineD.lib").string();
#endif

		File::CopyFileOrFolder(originLib, targetLib);

		string gameBuilder = (path(gameRootFolder) / "build_windows.bat").string();
#elif __linux__

#ifdef NDEBUG
		releaseType = "release";
		originLib = (path(engineRootFolder) / "libElypso engine.a").string();
		targetLib = (path(gameRootFolder) / "libElypso engine.a").string();
#else
		releaseType = "debug";
		originLib = (path(engineRootFolder) / "libElypso engineD.a").string();
		targetLib = (path(gameRootFolder) / "libElypso engineD.a").string();
#endif

		File::CopyFileOrFolder(originLib, targetLib);

		string gameBuilder = (path(gameRootFolder) / "build_linux.sh").string();
#endif

		string command = "";

		switch (installerType)
		{
		case InstallerType::reset:
		{
#ifdef _WIN32
			command = "cmd /c \"" + gameBuilder + "\" cmake " + releaseType + " skipwait";
#elif __linux__
			command = "bash \"" + gameBuilder + "\" cmake " + releaseType + " skipwait";
#endif
			break;
		}
		case InstallerType::compile:
		{
#ifdef _WIN32
			command = "cmd /c \"" + gameBuilder + "\" build " + releaseType + " skipwait";
#elif __linux__
			command = "bash \"" + gameBuilder + "\" build " + releaseType + " skipwait";
#endif
			break;
		}
		}

		//command to run the batch file and capture errors
		string fullCommand = command + " 2>&1"; //redirect stderr to stdout
		cout << "!!!! release type: " << releaseType << "\n";
		cout << "!!!! COMMAND: " << command << "\n";

		array<char, 128> buffer{};
#ifdef _WIN32
		unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(fullCommand.c_str(), "r"), _pclose);

		if (!pipe)
		{
			throw runtime_error("_popen() failed!");
			return false;
		}

		//read the output line by line and add to the provided vector
		while (fgets(
			buffer.data(),
			static_cast<int>(buffer.size()),
			pipe.get()) != nullptr)
		{
			output.emplace_back(buffer.data());
			cout << buffer.data() << "\n";
		}
#elif __linux__
		auto pipe = unique_ptr<FILE, void(*)(FILE*)>(
			popen(fullCommand.c_str(), "r"),
			[](FILE* file) { if (file) pclose(file); }
		);

		if (!pipe)
		{
			throw runtime_error("popen() failed!");
		}

		//read the output line by line and add to the provided vector
		while (fgets(
			buffer.data(),
			static_cast<int>(buffer.size()),
			pipe.get()) != nullptr)
		{
			output.emplace_back(buffer.data());
			cout << buffer.data() << "\n";
		}
#endif
		return true;
	}

	void Compilation::RenderBuildingWindow()
	{
		ImVec2 windowSize = ImVec2(600.0f, 600.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImGui::SetNextWindowSizeConstraints(ImVec2(500.0f, 500.0f), ImVec2(1500.0f, 1500.0f));

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (renderBuildingWindow
			&& ImGui::Begin("##Building", NULL, windowFlags))
		{
			string gameName = ConfigFile::GetValue("gameName");

			string text = !finishedBuild 
				? "Building " + gameName
				: "Finished building " + gameName;
			ImGui::Text("%s", text.c_str());

			ImVec2 scrollingRegionSize(
				ImGui::GetContentRegionAvail().x,
				ImGui::GetContentRegionAvail().y - 75);
			if (ImGui::BeginChild("ScrollingRegion", scrollingRegionSize, true))
			{
				float wrapWidth = ImGui::GetContentRegionAvail().x - 10;
				ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);

				//display the content of the text buffer
				for (const auto& message : output)
				{
					ImGui::TextWrapped("%s", message.c_str());

					/*
					* 
					* DISABLED COPYING BUILD WINDOW TEXT
					* UNTIL I BOTHER TO FIX THE ODD CRASH
					* THAT THIS CAUSES IN VS2022 IN DEBUG MODE
					* WHEN I BUILD A GAME
					* 
					if (ImGui::IsItemClicked()
						&& ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::INPUT,
							Type::DEBUG,
							"Added '" + message + "' to clipboard.\n");
						ImGui::SetClipboardText(message.c_str());
					}
					*/
				}

				ImGui::PopTextWrapPos();

				//scrolls to the bottom if scrolling is allowed
				//and if user is close to the newest compilation message
				bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f;
				if (isNearBottom
					|| (!firstScrollToBottom
					&& Engine::isEngineRunning))
				{
					ImGui::SetScrollHereY(1.0f);
					firstScrollToBottom = true;
				}
			}

			ImGui::EndChild();

			if (finishedBuild)
			{
				ImVec2 windowSize = ImGui::GetWindowSize();

				ImVec2 buttonSize(120, 50);

				ImVec2 button1Pos(
					(windowSize.x * 0.2f) - (buttonSize.x * 0.5f),
					windowSize.y - buttonSize.y - 15.0f);
				ImVec2 button2Pos(
					(windowSize.x * 0.5f) - (buttonSize.x * 0.5f),
					windowSize.y - buttonSize.y - 15.0f);
				ImVec2 button3Pos(
					(windowSize.x * 0.8f) - (buttonSize.x * 0.5f),
					windowSize.y - buttonSize.y - 15.0f);

				ImGui::SetCursorPos(button1Pos);
				if (ImGui::Button("Close", buttonSize))
				{
					renderBuildingWindow = false;
				}

				ImGui::SetCursorPos(button2Pos);
				if (ImGui::Button("Run game", buttonSize))
				{
					renderBuildingWindow = false;

					if (GamePathCheck())
					{
						File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
					}
				}

				ImGui::SetCursorPos(button3Pos);
				if (ImGui::Button("Clean rebuild", buttonSize))
				{
					string gameBatPath = (path(Engine::gamePath) / "quickBuild.bat").string();

					installerType = InstallerType::reset;
					Compile();
				}
			}

			ImGui::End();
		}
	}

	void Compilation::Run()
	{
		string gameName = path(Engine::gameExePath).stem().string();

		string projectFolder = (path(Engine::gameParentPath) / "project").string();

		if (!exists(projectFolder))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Game root folder doesn't exist! Did you forget to compile?\n");
		}
		else
		{
			if (!exists(Engine::gameExePath))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: Game exe does not exist! Did you forget to compile?\n");
			}
			else
			{
				SceneFile::SaveScene();

				//
				// CREATE NEW GAME DOCUMENTS FOLDER AND PLACE ALL SCENES AND THEIR CONTENT TO IT
				//

				if (exists(projectFolder)) File::DeleteFileOrfolder(path(projectFolder) / "scenes");

				string engineProjectFolder = path(Engine::projectPath).string();
				for (const auto& entry : directory_iterator(path(engineProjectFolder)))
				{
					string stem = path(entry).stem().string();

					if (stem != "models"
						&& stem != "audio"
						&& stem != "textures"
						&& stem != "project")
					{
						string origin = path(entry).string();
						string originFileName = path(entry).filename().string();
						string target = (path(projectFolder) / originFileName).string();

						File::CopyFileOrFolder(origin, target);
					}
				}

				File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
			}
		}
	}

	bool Compilation::GamePathCheck()
	{
		string output = "Checking if game exe path '" + Engine::gameExePath + "' is valid.\n";
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			output);

		//first check if msvc is valid
		if (!exists(Engine::gameExePath))
		{
			string parentFolder = path(Engine::gameExePath).parent_path().parent_path().string();
			string gameName = path(Engine::gameExePath).filename().string();
			string newPath = (path(parentFolder) / "msvc" / gameName).string();

			output = "Failed to find game exe at '" + Engine::gameExePath + "'. Trying another compiler folder path.\n";
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::DEBUG,
				output);
		}
		return true;
	}
}
#endif
