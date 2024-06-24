//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_importAsset.hpp"
#include "gui.hpp"
#include "model.hpp"
#include "sceneFile.hpp"
#include "render.hpp"
#include "core.hpp"
#include "fileexplorer.hpp"
#include "gameobject.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "texture.hpp"
#include "console.hpp"

using std::filesystem::exists;
using std::filesystem::path;
using std::exception;
using std::filesystem::directory_iterator;

using Graphics::Shape::Model;
using EngineFile::SceneFile;
using Graphics::Render;
using Core::Engine;
using EngineFile::FileExplorer;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::GameObject;
using Utils::String;
using Utils::File;
using Graphics::Texture;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::GUI
{
	void GUIImportAsset::RenderImportAsset()
	{
		ImVec2 pos = ImVec2(400, 400);
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoResize;

		if (renderImportAsset
			&& ImGui::Begin("Import asset", NULL, windowFlags))
		{
			RenderImportAssetContent();

			ImGui::End();
		}
	}

	void GUIImportAsset::RenderImportAssetContent()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 textfieldPos = ImVec2(
			windowSize.x / 2 - 125,
			windowSize.y / 2 - 50);

		ImGui::SetCursorPos(textfieldPos);

		strcpy_s(name, bufferSize, newName.c_str());
		if (ImGui::InputText("##setSceneName", name, bufferSize))
		{
			newName = name;
		}

		if (strlen(name) == 0) strcpy_s(name, bufferSize, "_");
		if (newName == "") newName = "_";

		ImVec2 buttonSize = ImVec2(100, 30);

		//import asset into engine
		ImVec2 importButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(importButtonPos);
		if (ImGui::Button("Import", buttonSize))
		{
			string stem = path(assetPath).stem().string();
			string extension = path(assetPath).extension().string();

			//
			// CHECK IF NAME HAS ILLEGAL CHARACTER
			//

			bool foundIllegalChar = false;
			for (char c : newName)
			{
				if (!String::IsValidSymbolInPath(c))
				{
					foundIllegalChar = true;
					break;
				}
			}
			if (foundIllegalChar)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Invalid character detected in file name '" + newName + "'! Please only use english letters, roman numbers and dash, dot or underscore symbol!");

				strcpy_s(name, bufferSize, "Name");
				newName = "Name";

				renderImportAsset = false;

				return;
			}

			//
			// CHECK IF FILE WITH SAME NAME ALREADY EXISTS
			//

			bool foundExistingFile = false;

			string cleanedModelsFolder = String::StringReplace(
				path(Engine::scenePath).parent_path().string() + "\\models", "/", "\\");
			string cleanedTexturesFolder = String::StringReplace(
				path(Engine::scenePath).parent_path().string() + "\\textures", "/", "\\");

			for (const auto& entry : directory_iterator(cleanedModelsFolder))
			{
				if (entry.path().filename().string() == newName)
				{
					foundExistingFile = true;
					break;
				}
			}

			for (const auto& entry : directory_iterator(cleanedTexturesFolder))
			{
				if (entry.path().filename().string() == newName)
				{
					foundExistingFile = true;
					break;
				}
			}

			if (foundExistingFile)
			{
				strcpy_s(name, bufferSize, newName.c_str());
				newName = newName;

				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"File name '" + newName + "' already exists in this project! Please pick a new file name.");

				renderImportAsset = false;

				return;
			}

			//
			// IMPORT MODEL
			//

			if (extension == ".fbx"
				|| extension == ".gltw"
				|| extension == ".obj")
			{
				string scenePath = path(Engine::scenePath).parent_path().string();
				string newFilePath = scenePath + "/models/" + path(assetPath).filename().string();
				File::CopyFileOrFolder(assetPath, newFilePath);

				Model::targetModel = newFilePath;
				Model::Initialize(
					vec3(0),
					vec3(0),
					vec3(1),
					newFilePath,
					Engine::filesPath + "/shaders/GameObject.vert",
					Engine::filesPath + "/shaders/GameObject.frag",
					Engine::filesPath + "/textures/diff_default.png",
					"EMPTY",
					"EMPTY",
					"EMPTY",
					32,
					newName,
					Model::tempID);

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			//
			// IMPORT TEXTURE
			//

			else if (extension == ".png"
				|| extension == ".jpg"
				|| extension == ".jpeg")
			{
				string scenePath = path(Engine::scenePath).parent_path().string();
				string newFilePath = scenePath + "/textures/" + path(assetPath).filename().string();
				File::CopyFileOrFolder(assetPath, newFilePath);
			}

			renderImportAsset = false;
		}
		ImVec2 cancelButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 + buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(cancelButtonPos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			renderImportAsset = false;
		}
	}
}