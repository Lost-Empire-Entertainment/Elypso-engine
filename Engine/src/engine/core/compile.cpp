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
#include "gameobject.hpp"
#include "render.hpp"
#include "sceneFile.hpp"
#include "gui_engine.hpp"
#include "gui_settings.hpp"
#include "configFile.hpp"
#include "fileutils.hpp"

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
using std::filesystem::path;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Render;
using EngineFile::SceneFile;
using Graphics::GUI::EngineGUI;
using Graphics::GUI::GUISettings;
using EngineFile::ConfigFile;
using Utils::File;

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

				if (exists(path(Engine::gameExePath).parent_path()))
				{
					for (const auto& file : directory_iterator(path(Engine::gameExePath).parent_path()))
					{
						string filePath = path(file).string();
						string fileExtension = path(file).extension().string();

						if (fileExtension == ".exe")
						{
							File::DeleteTarget(filePath);
						}
					}
				}

				//
				// START BUILDING GAME FROM SOURCE CODE
				//

				if (RunInstaller())
				{
					//
					// COPY PROJECT FILE TO GAME DOCUMETS FOLDER
					//

					string targetFolder = ((path(Engine::gameExePath).parent_path()) / "project").string();
					if (exists(targetFolder)) File::DeleteTarget(targetFolder);
					File::CreateNewFolder(targetFolder);

					string projectFileOriginPath = (path(Engine::docsPath) / "project.txt").string();
					string projectFileTargetPath = (path(targetFolder) / "project.txt").string();
					if (exists(projectFileTargetPath)) File::DeleteTarget(projectFileTargetPath);
					File::CopyTarget(projectFileOriginPath, projectFileTargetPath);

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

							File::CopyTarget(origin, target);
						}
					}

					//
					// CREATE FIRST SCENE FILE WHICH GAME LOADS FROM WHEN GAME EXE IS RAN
					//

					string firstSceneFilePath = (path(targetFolder) / "firstScene.txt").string();
					if (exists(firstSceneFilePath)) File::DeleteTarget(firstSceneFilePath);

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
		string engineLibRootFolder{};
		string engineRootFolder{};
		string gameRootFolder{};

		string parentFolder = current_path().stem().string();
		//if engine is ran from repository or visual studio folder
		string releaseFolderName = "build-release";
		string debugFolderName = "build-debug";

		if (parentFolder == releaseFolderName
			|| parentFolder == debugFolderName)
		{
#ifdef NDEBUG
			engineLibRootFolder = (current_path()
				.parent_path()
				.parent_path()
				/ "Engine library"
				/ releaseFolderName
				).string();
#else
			engineLibRootFolder = (current_path()
				.parent_path()
				.parent_path()
				/ "Engine library"
				/ debugFolderName
				).string();
#endif
			engineRootFolder = (current_path()
				.parent_path()).string();
			gameRootFolder = (current_path()
				.parent_path()
				.parent_path() / "Game").string();
		}
		else if (parentFolder == "Engine")
		{
			engineRootFolder = current_path().string();
			gameRootFolder = (current_path()
				.parent_path() / "Game").string();
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

		string libStart{};
		string libEngineEnd{};
		string libGameEnd{};
		string releaseType{};
		string gameBuilder{};

#ifdef _WIN32

#ifdef NDEBUG
		releaseType = "release";
		libStart = (path(engineLibRootFolder) / "Elypso engine.lib").string();
		libEngineEnd = (path(engineRootFolder) / "Elypso engine.lib").string();
		libGameEnd = (path(gameRootFolder) / "Elypso engine.lib").string();
		gameBuilder = (path(gameRootFolder) / "build_windows_release.bat").string();
#else
		releaseType = "debug";
		libStart = (path(engineLibRootFolder) / "Elypso engineD.lib").string();
		libEngineEnd = (path(engineRootFolder) / "Elypso engineD.lib").string();
		libGameEnd = (path(gameRootFolder) / "Elypso engineD.lib").string();
		gameBuilder = (path(gameRootFolder) / "build_windows_debug.bat").string();
#endif

#elif __linux__

#ifdef NDEBUG
		releaseType = "release";
		libStart = (path(engineLibRootFolder) / "libElypso engine.a").string();
		libEngineEnd = (path(engineRootFolder) / "libElypso engine.a").string();
		libGameEnd = (path(gameRootFolder) / "libElypso engine.a").string();
		gameBuilder = (path(gameRootFolder) / "build_linux_release.sh").string();
#else
		releaseType = "debug";
		libStart = (path(engineLibRootFolder) / "libElypso engineD.a").string();
		libEngineEnd = (path(engineRootFolder) / "libElypso engineD.a").string();
		libGameEnd = (path(gameRootFolder) / "libElypso engineD.a").string();
		gameBuilder = (path(gameRootFolder) / "build_linux_debug.sh").string();
#endif

#endif

		if (!exists(libStart))
		{
			string output = "Engine library '" + libStart + "' does not exist!\n";
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				output);
			return false;
		}

		File::CopyTarget(libStart, libEngineEnd);
		File::CopyTarget(libStart, libGameEnd);

		string command{};

		switch (installerType)
		{
		case InstallerType::reset:
		{
#ifdef _WIN32
			command = "cmd /c \"" + gameBuilder + " reset\"";
#elif __linux__
			command = "bash \"" + gameBuilder + "\" reset";
#endif
			break;
		}
		case InstallerType::compile:
		{
#ifdef _WIN32
			command = "cmd /c \"" + gameBuilder + "\"";
#elif __linux__
			command = "bash \"" + gameBuilder + "\"";
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
		int fbWidth{};
		int fbHeight{};
		glfwGetFramebufferSize(Render::window, &fbWidth, &fbHeight);

		ImVec2 windowSize = ImVec2(500.0f, 500.0f);
		if (windowSize.x > fbWidth - 50.0f) windowSize.x = fbWidth - 50.0f;
		if (windowSize.y > fbHeight - 50.0f) windowSize.x = fbHeight - 50.0f;
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 300.0f), ImVec2(fbWidth - 50.0f, fbHeight - 50.0f));

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
					&& Engine::isRunning))
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

					Run();
				}

				ImGui::SetCursorPos(button3Pos);
				if (ImGui::Button("Clean rebuild", buttonSize))
				{
					string gameBatPath = (path(Engine::gameRootFolder) / "quickBuild.bat").string();

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

		string projectFolder = (path(Engine::gameExePath).parent_path() / "project").string();

		if (!exists(projectFolder))
		{
			Engine::CreateWarningPopup("Game root folder doesn't exist! Did you forget to compile?");
		}
		else
		{
			if (!exists(Engine::gameExePath))
			{
				Engine::CreateWarningPopup("Game exe does not exist! Did you forget to compile?");
			}
			else
			{
				SceneFile::SaveScene();

				//
				// CREATE NEW GAME DOCUMENTS FOLDER AND PLACE ALL SCENES AND THEIR CONTENT TO IT
				//

				if (exists(projectFolder)) File::DeleteTarget((path(projectFolder) / "scenes").string());

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

						File::CopyTarget(origin, target);
					}
				}

				File::RunApplication(Engine::gameExePath);
			}
		}
	}
}
#endif
