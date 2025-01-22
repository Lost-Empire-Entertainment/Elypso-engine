//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <filesystem>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_importAsset.hpp"
#include "gui_engine.hpp"
#include "importer.hpp"
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

using Graphics::Shape::Importer;
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
	static bool assignedName;

	void GUIImportAsset::RenderImportAsset()
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

		if (!assignedName)
		{
			string assetName = path(assetPath).stem().string();
			int assetNameLength = static_cast<int>(assetName.size());
			if (assetNameLength > 16) assetName = assetName.substr(0, 15);
			newName = assetName;

#ifdef _WIN32
			strcpy_s(name, bufferSize, newName.c_str());
#elif __linux__
			strncpy(name, newName.c_str(), bufferSize);
#endif
			assignedName = true;
		}

#ifdef _WIN32
		strcpy_s(name, bufferSize, newName.c_str());
#elif __linux__
		strncpy(name, newName.c_str(), bufferSize);
#endif
		if (ImGui::InputText("##setSceneName", name, bufferSize))
		{
			newName = name;
		}

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
					Caller::INPUT,
					Type::EXCEPTION,
					"Error: Invalid character detected in file name '" + newName + "'! Please only use english letters, roman numbers and dash, dot or underscore symbol!");

#ifdef _WIN32
				strcpy_s(name, bufferSize, path(assetPath).filename().string().c_str());
#elif __linux__
				strncpy(name, path(assetPath).filename().string().c_str(), bufferSize);
#endif
				newName = path(assetPath).filename().string();
				renderImportAsset = false;

				return;
			}

			//
			// IMPORT MODEL
			//

			if (extension == ".fbx"
				|| extension == ".gltf"
				|| extension == ".obj")
			{
				string targetFolder = (path(Engine::projectPath) / "models").string();
				if (!exists(targetFolder)) File::CreateNewFolder(targetFolder);
				string assetName = path(assetPath).filename().string();
				string targetPath = (path(targetFolder) / assetName).string();

				if (exists(targetPath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Model '" + assetName + "' already exists in this project!\n");
				}
				else 
				{
					File::CopyFileOrFolder(assetPath, targetPath);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}

			//
			// IMPORT TEXTURE
			//

			else if (extension == ".png"
					 || extension == ".jpg"
					 || extension == ".jpeg")
			{
				string textureFilename = newName + extension;
				string texturesFolder = Engine::texturesPath;

				string newFilePath = 
					(path(texturesFolder) / textureFilename).string();
				File::CopyFileOrFolder(assetPath, newFilePath);

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			renderImportAsset = false;
			assignedName = false;
		}
		ImVec2 cancelButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 + buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(cancelButtonPos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			renderImportAsset = false;
			assignedName = false;
		}
	}
}
#endif
