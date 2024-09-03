//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <filesystem>
#include <thread>
#include <iostream>
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
#include "gui_settings.hpp"
#include "sceneFile.hpp"
#include "gui.hpp"

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

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using Utils::String;
using Utils::File;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::Mesh;
using Graphics::Render;
using Graphics::GUI::GUISettings;
using EngineFile::SceneFile;
using Graphics::GUI::EngineGUI;

namespace Core
{
	void Compilation::Compile()
	{
		renderBuildingWindow = true;

		SceneFile::SaveScene();

		thread CompileThread([]()
			{
				//
				// START BUILDING GAME FROM SOURCE CODE
				//

				string gameBatPath = Engine::gamePath + "\\quickBuild.bat";
				gameBatPath = String::CharReplace(gameBatPath, '/', '\\');
				if (!exists(gameBatPath)) return;

				RunInstaller(gameBatPath);

				string gameStem = path(Engine::gameExePath).stem().string();
				if (gameStem != "Game")
				{
					File::MoveOrRenameFileOrFolder(
						Engine::gameParentPath + "\\Game.exe",
						Engine::gameExePath,
						true);
				}

				//
				// CREATE NEW GAME DOCUMENTS FOLDER AND PLACE ALL SCENES TO IT
				//

				string gameName = path(Engine::gameExePath).stem().string();
				string myGamesFolder = path(Engine::docsPath).parent_path().string() + "\\My Games";
				if (!exists(myGamesFolder)) File::CreateNewFolder(myGamesFolder);

				string gameDocsFolder = myGamesFolder + "\\" + gameName;
				if (exists(gameDocsFolder)) File::DeleteFileOrfolder(gameDocsFolder);

				File::CreateNewFolder(gameDocsFolder);

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
						string target = gameDocsFolder + "\\" + originFileName;

						File::CopyFileOrFolder(origin, target);
					}
				}

				//
				// CREATE FIRST SCENE FILE WHICH GAME LOADS FROM WHEN GAME EXE IS RAN
				//

				string firstSceneFilePath = gameDocsFolder + "\\firstScene.txt";
				if (exists(firstSceneFilePath)) File::DeleteFileOrfolder(firstSceneFilePath);

				ofstream firstSceneFile(firstSceneFilePath);
				if (!firstSceneFile.is_open())
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Compilation failed because first scene file couldnt be created!\n");

					renderBuildingWindow = false;

					return;
				}

				firstSceneFile << "project= " << path(Engine::projectPath).stem().string() << "\n";
				firstSceneFile << "scene= " << Engine::gameFirstScene;

				firstSceneFile.close();

				//
				// FINISHED COMPILATION
				//

				renderBuildingWindow = false;

				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::INFO,
					"Compilation succeeded! Running game.\n");

				if (!exists(Engine::gameExePath)
					|| !exists(Engine::gameParentPath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Failed to find game template folder or game exe!\n");
				}
				else File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
			});

		CompileThread.detach();
	}
	
	void Compilation::RunInstaller(const string& installer)
	{
		string command = "\"" + installer + "\"";

		//command to run the batch file and capture errors
		string fullCommand = command + " 2>&1"; //redirect stderr to stdout

		array<char, 128> buffer{};
		unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(fullCommand.c_str(), "r"), _pclose);

		if (!pipe)
		{
			throw runtime_error("_popen() failed!");
		}

		//read the output line by line and add to the provided vector
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		{
			output.emplace_back(buffer.data());
			cout << buffer.data() << "\n";
		}
	}

	void Compilation::RenderBuildingWindow()
	{
		ImVec2 windowSize = ImVec2(300.0f, 300.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (renderBuildingWindow
			&& ImGui::Begin("##Building", NULL, windowFlags))
		{
			string text = "Building " + GUISettings::gameName + "...";
			ImGui::Text(text.c_str());

			ImVec2 scrollingRegionSize(
				ImGui::GetContentRegionAvail().x,
				ImGui::GetContentRegionAvail().y - 40);
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
						&& Engine::startedWindowLoop))
				{
					ImGui::SetScrollHereY(1.0f);
					firstScrollToBottom = true;
				}
			}

			ImGui::EndChild();

			ImGui::End();
		}
	}
}