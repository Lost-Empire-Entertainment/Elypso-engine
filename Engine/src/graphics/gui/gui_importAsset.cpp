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

using std::filesystem::exists;
using std::filesystem::path;
using std::exception;

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

namespace Graphics::GUI
{
	void GUIImportAsset::RenderImportAsset()
	{
		ImVec2 minSize = ImVec2(600, 600);
		ImVec2 pos = ImVec2(400, 400);
		ImGui::SetNextWindowSizeConstraints(minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (renderImportAsset
			&& ImGui::Begin("Import asset", NULL, windowFlags))
		{
			ImVec2 buttonSize = ImVec2(100, 30);

			ImGui::Text("Name");
			strcpy_s(
				inputTextBuffer_objName, 
				nameBufferSize, 
				assignedName.c_str());
			ImGui::InputText(
				"##objName", 
				inputTextBuffer_objName,
				nameBufferSize);
			if (ImGui::IsItemEdited()) assignedName = inputTextBuffer_objName;

			ImGui::Spacing();

			//import asset into engine
			ImVec2 importButtonPos = ImVec2(
				ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2, 
				ImGui::GetWindowSize().y - 50);
			ImGui::SetCursorPos(importButtonPos);
			if (ImGui::Button("Import", buttonSize))
			{
				string extension = path(assetPath).extension().string();

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
						assignedName,
						Model::tempID);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

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

			ImGui::End();
		}
	}
}