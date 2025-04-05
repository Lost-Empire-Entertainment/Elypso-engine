//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <iostream>
#include <fstream>
#include <filesystem>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_createscene.hpp"
#include "gui_engine.hpp"
#include "sceneFile.hpp"
#include "render.hpp"
#include "core.hpp"
#include "console.hpp"
#include "fileutils.hpp"
#include "stringutils.hpp"

using std::cout;
using std::ofstream;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::path;

using EngineFile::SceneFile;
using Graphics::Render;
using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::String;
using Utils::File;

namespace Graphics::GUI
{
	void GUICreateScene::RenderCreateSceneWindow()
	{
		ImVec2 windowSize = ImVec2(400.0f, 400.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoResize;

		if (renderCreateSceneWindow
			&& ImGui::Begin("Create scene", NULL, windowFlags))
		{
			RenderCreateSceneWindowContent();

			ImGui::End();
		}
	}

	void GUICreateScene::RenderCreateSceneWindowContent()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 textfieldPos = ImVec2(
			windowSize.x / 2 - 125,
			windowSize.y / 2 - 50);

		ImGui::SetCursorPos(textfieldPos);

#ifdef _WIN32
		strcpy_s(sceneName, bufferSize, assignedSceneName.c_str());
#elif __linux__
		strncpy(sceneName, assignedSceneName.c_str(), bufferSize);
#endif
		if (ImGui::InputText("##setSceneName", sceneName, bufferSize))
		{
			assignedSceneName = sceneName;
		}

		if (strlen(sceneName) == 0)
		{
#ifdef _WIN32
			strcpy_s(sceneName, bufferSize, "Scene");
#elif __linux__
			strncpy(sceneName, "Scene", bufferSize);
#endif
		}
		if (assignedSceneName == "") assignedSceneName = "Scene";

		ImVec2 buttonSize = ImVec2(100, 30);

		ImVec2 createButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(createButtonPos);
		if (ImGui::Button("Create", buttonSize))
		{
			//
			// CHECK IF NAME HAS ILLEGAL CHARACTER
			//

			bool foundIllegalChar = false;
			char illegalChar;
			for (char c : assignedSceneName)
			{
				if (!File::IsValidSymbolInPath(c))
				{
					foundIllegalChar = true;
					illegalChar = c;
					break;
				}
			}
			if (foundIllegalChar)
			{
				string str(1, illegalChar);

				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::EXCEPTION,
					"Error: Invalid character '" + str + "' detected in scene name '" + assignedSceneName + "'! Please only use english letters, roman numbers and dash or underscore symbol!");

#ifdef _WIN32
				strcpy_s(sceneName, bufferSize, "Scene");
#elif __linux__
				strncpy(sceneName,"Scene", bufferSize);
#endif
				
				assignedSceneName = "Scene";

				renderCreateSceneWindow = false;

				return;
			}
			
			//
			// CHECK IF SCENE FOLDER WITH SAME NAME ALREADY EXISTS
			//

			bool foundExistingScene = false;

			for (const auto& entry : directory_iterator(Engine::scenesPath))
			{
				if (entry.path().filename().string() == assignedSceneName)
				{
					foundExistingScene = true;
					break;
				}
			}

			if (foundExistingScene)
			{
#ifdef _WIN32
				strcpy_s(sceneName, bufferSize, "Scene");
#elif __linux__
				strncpy(sceneName, "Scene", bufferSize);
#endif
				assignedSceneName = "Scene";

				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::EXCEPTION,
					"Error: Scene name '" + assignedSceneName + "' already exists in this project! Please pick a new scene name.");

				renderCreateSceneWindow = false;

				return;
			}

			//
			// CREATE NEW SCENE
			//

			string newSceneFolder = (path(Engine::scenesPath) / assignedSceneName).string();
			File::CreateNewFolder(newSceneFolder);
			File::CreateNewFolder((path(newSceneFolder) / "gameobjects").string());

			string newSceneFile = (path(newSceneFolder) / "scene.txt").string();

			ofstream sceneFile(newSceneFile);
			sceneFile.close();

			SceneFile::LoadScene(newSceneFile);

			Render::SetWindowNameAsUnsaved(false);

			renderCreateSceneWindow = false;
		}

		ImVec2 cancelButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 + buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(cancelButtonPos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			renderCreateSceneWindow = false;
		}
	}
}
#endif