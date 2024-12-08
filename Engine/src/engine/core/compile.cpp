//Copyright(C) 2024 Lost Empire Entertainment
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
#include "meshcomponent.hpp"

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

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using Utils::String;
using Utils::File;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Components::Mesh;
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
				RunInstaller();

				string gameStem = path(Engine::gameExePath).stem().string();
				if (gameStem != "Game")
				{
					File::MoveOrRenameFileOrFolder(
						path(Engine::gameParentPath) / "Game.exe",
						Engine::gameExePath,
						true);
				}

				//
				// CREATE NEW GAME DOCUMENTS FOLDER AND PLACE ALL SCENES TO IT
				//

				string myGamesFolder = (path(Engine::docsPath).parent_path() / "My Games").string();
				if (!exists(myGamesFolder)) File::CreateNewFolder(myGamesFolder);

				string gameName = ConfigFile::GetValue("gameName");

				string gameDocsFolder = (path(myGamesFolder) / gameName).string();
				if (exists(gameDocsFolder)) File::DeleteFileOrfolder(gameDocsFolder);

				File::CreateNewFolder(gameDocsFolder);

				//
				// COPY PROJECT FILE TO GAME DOCUMETS FOLDER
				//

				string projectFileOriginPath = (path(Engine::docsPath) / "project.txt").string();
				string projectFileTargetPath = (path(gameDocsFolder) / "project.txt").string();
				if (exists(projectFileTargetPath)) File::DeleteFileOrfolder(projectFileTargetPath);
				File::CopyFileOrFolder(projectFileOriginPath, projectFileTargetPath);

				//
				// COPY SCENE FILES TO GAME DOCUMENTS FOLDER
				//

				string scenePath = path(Engine::projectPath).parent_path().string();
				for (const auto& entry : directory_iterator(path(scenePath)))
				{
					string stem = path(entry).stem().string();

					if (stem != "models"
						&& stem != "textures"
						&& stem != "project")
					{
						string origin = path(entry).string();
						string originFileName = path(entry).filename().string();
						string target = (path(gameDocsFolder) / originFileName).string();

						File::CopyFileOrFolder(origin, target);
					}
				}

				//
				// CREATE FIRST SCENE FILE WHICH GAME LOADS FROM WHEN GAME EXE IS RAN
				//

				string firstSceneFilePath = (path(gameDocsFolder) / "firstScene.txt").string();
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
			});

		CompileThread.detach();
	}
	
	void Compilation::RunInstaller()
	{
		string engineRootFolder = "";
		string gameRootFolder = "";

		string parentFolderStem = current_path().stem().string();
		if (parentFolderStem == "x64-release"
			|| parentFolderStem == "x64-debug")
		{
			engineRootFolder = (current_path().parent_path().parent_path().parent_path()).string();
			gameRootFolder = (current_path().parent_path().parent_path().parent_path().parent_path() / "Game").string();
		}
		else if (parentFolderStem == "Engine")
		{
			engineRootFolder = (current_path().parent_path()).string();
			gameRootFolder = (current_path().parent_path().parent_path() / "Game").string();
		}

		if (gameRootFolder == ""
			|| !exists(gameRootFolder))
		{
			Engine::CreateErrorPopup("Failed to assign path to game builder!");
		}

#ifdef _WIN32
		string origin = (path(engineRootFolder) / "Elypso engine.lib").string();
		string target = (path(gameRootFolder) / "Elypso engine.lib").string();
		File::CopyFileOrFolder(origin, target);

		string gameBuilder = (path(gameRootFolder) / "build_windows.bat").string();
#elif __linux__
		string origin = (path(engineRootFolder) / "libElypso engine.a").string();
		string target = (path(gameRootFolder) / "libElypso engine.a").string();
		File::CopyFileOrFolder(origin, target);

		string gameBuilder = (path(gameRootFolder) / "build_linux.sh").string();
#endif

		string command = "";
		switch (installerType)
		{
		case InstallerType::reset:
		{
#ifdef _WIN32
			command = "cmd /c \"" + gameBuilder + "\" cmake skipwait";
#elif __linux__
			command = "bash \"" + gameBuilder + "\" cmake skipwait";
#endif
			break;
		}
		case InstallerType::compile:
		{
#ifdef _WIN32
			command = "cmd /c \"" + gameBuilder + "\" build skipwait";
#elif __linux__
			command = "bash \"" + gameBuilder + "\" build skipwait";
#endif
			break;
		}
		}

		//command to run the batch file and capture errors
		string fullCommand = command + " 2>&1"; //redirect stderr to stdout

		array<char, 128> buffer{};
#ifdef _WIN32
		unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(fullCommand.c_str(), "r"), _pclose);

		if (!pipe)
		{
			throw runtime_error("_popen() failed!");
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

					if (ImGui::IsItemClicked()
						&& ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::INPUT,
							Type::DEBUG,
							"Added '" + message + "' to clipboard.\n");
						ImGui::SetClipboardText(message.c_str());
					}
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

					cout << "Attempting to run game from " << Engine::gameExePath << "\n";

					if (!exists(Engine::gameExePath)
						|| !exists(Engine::gameParentPath))
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::FILE,
							Type::EXCEPTION,
							"Error: Failed to find game template folder or game exe!\n");
					}
					else File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
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
		string myGamesFolder = (path(Engine::docsPath).parent_path() / "My Games").string();
		string gameProjectFolder = (path(myGamesFolder) / gameName / path(Engine::projectPath).stem()).string();

		if (!exists(path(gameProjectFolder).parent_path()))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Game documents folder doesn't exist! Did you forget to compile?\n");
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

				if (exists(gameProjectFolder)) File::DeleteFileOrfolder(path(gameProjectFolder) / "scenes");

				string engineProjectFolder = path(Engine::projectPath).string();
				for (const auto& entry : directory_iterator(path(engineProjectFolder)))
				{
					string stem = path(entry).stem().string();

					if (stem != "models"
						&& stem != "textures"
						&& stem != "project")
					{
						string origin = path(entry).string();
						string originFileName = path(entry).filename().string();
						string target = (path(gameProjectFolder) / originFileName).string();

						File::CopyFileOrFolder(origin, target);
					}
				}

				File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
			}
		}
	}
}
#endif