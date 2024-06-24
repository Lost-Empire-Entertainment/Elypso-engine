//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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
#include "gui.hpp"
#include "sceneFile.hpp"
#include "render.hpp"
#include "fileUtils.hpp"
#include "core.hpp"
#include "stringUtils.hpp"
#include "console.hpp"

using std::cout;
using std::ofstream;
using std::filesystem::exists;
using std::filesystem::directory_iterator;

using EngineFile::SceneFile;
using Graphics::Render;
using Utils::File;
using Core::Engine;
using Utils::String;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::GUI
{
	void GUICreateScene::RenderCreateSceneWindow()
	{
		ImVec2 pos = ImVec2(400, 400);
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);

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

		strcpy_s(sceneName, bufferSize, assignedSceneName.c_str());
		if (ImGui::InputText("##setSceneName", sceneName, bufferSize))
		{
			assignedSceneName = sceneName;
		}

		if (strlen(sceneName) == 0) strcpy_s(sceneName, bufferSize, "_");
		if (assignedSceneName == "") assignedSceneName = "_";

		ImVec2 buttonSize = ImVec2(100, 30);

		ImVec2 createButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(createButtonPos);
		if (ImGui::Button("Create", buttonSize))
		{
			bool foundIllegalChar = false;
			for (char c : assignedSceneName)
			{
				if (!String::IsValidSymbolInPath(c))
				{
					foundIllegalChar = true;
					break;
				}
			}
			if (foundIllegalChar)
			{
				strcpy_s(sceneName, bufferSize, "Scene");
				assignedSceneName = "Scene";

				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Invalid character detected in scene name '" + assignedSceneName + "'! Please only use english letters, roman numbers and dash, dot or underscore symbol!");
			}
			else
			{
				bool foundExistingScene = false;

				string parentPath = path(Engine::scenePath).parent_path().parent_path().string();

				for (const auto& entry : directory_iterator(parentPath))
				{
					if (entry.path().filename().string() == assignedSceneName)
					{
						foundExistingScene = true;
						break;
					}
				}

				if (foundExistingScene)
				{
					strcpy_s(sceneName, bufferSize, "Scene");
					assignedSceneName = "Scene";

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Scene name '" + assignedSceneName+ "' already exists in this project! Please pick a new scene name.");
				}
				else
				{
					string newSceneFolder = parentPath + "\\" + assignedSceneName;
					string newSceneFile = newSceneFolder + "\\scene.txt";

					File::CreateNewFolder(newSceneFolder);
					ofstream sceneFile(newSceneFile);
					sceneFile.close();

					SceneFile::LoadScene(newSceneFile);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

					renderCreateSceneWindow = false;
				}
			}
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