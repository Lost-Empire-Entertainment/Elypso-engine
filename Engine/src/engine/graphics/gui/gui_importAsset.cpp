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

using std::filesystem::exists;

using Graphics::Shape::Model;
using EngineFile::SceneFile;
using Graphics::Render;
using Core::Engine;
using EngineFile::FileExplorer;

namespace Graphics::GUI
{
	void GUIImportAsset::RenderImportAsset()
	{
		ImVec2 size = ImVec2(600, 600);
		ImVec2 pos = ImVec2(400, 400);
		ImGui::SetNextWindowSizeConstraints(size, size);
		ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoSavedSettings;

		if (renderImportAsset
			&& ImGui::Begin("Import asset", NULL, windowFlags))
		{
			ImVec2 buttonSize = ImVec2(100, 30);

			ImGui::Text("Name");
			strcpy_s(inputTextBuffer_objName, nameBufferSize, assignedName.c_str());
			ImGui::InputText("##objName", inputTextBuffer_objName, nameBufferSize);
			if (ImGui::IsItemEdited()) assignedName = inputTextBuffer_objName;

			ImGui::Spacing();

			ImGui::Text("Diffuse texture");
			string defaultDiffuseTexture = Engine::filesPath + "/textures/default_diffuse.png";
			if (assignedDiffuseTexture != ""
				&& exists(assignedDiffuseTexture))
			{
				strcpy_s(inputTextBuffer_diffTexturePath, pathBufferSize, assignedDiffuseTexture.c_str());
				defaultDiffuseTexture = assignedDiffuseTexture;
			}
			else
			{
				strcpy_s(inputTextBuffer_diffTexturePath, pathBufferSize, defaultDiffuseTexture.c_str());
			}
			ImGui::InputText("##diffTex", inputTextBuffer_diffTexturePath, pathBufferSize);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text(inputTextBuffer_diffTexturePath);
				ImGui::EndTooltip();
			}

			ImGui::SameLine();
			if (ImGui::Button("Select", buttonSize))
			{
				assignedDiffuseTexture = FileExplorer::Select(FileExplorer::SearchType::texture);
			}

			ImVec2 importButtonPos = ImVec2(300 - buttonSize.x - buttonSize.x / 2, 550);
			ImGui::SetCursorPos(importButtonPos);
			if (ImGui::Button("Import", buttonSize))
			{
				Model::targetModel = targetModelPath;
				Model::Initialize(
					vec3(0),
					vec3(0),
					vec3(1),
					targetModelPath,
					Engine::filesPath + "/shaders/GameObject.vert",
					Engine::filesPath + "/shaders/GameObject.frag",
					defaultDiffuseTexture,
					"EMPTY",
					"EMPTY",
					"EMPTY",
					32,
					assignedName,
					Model::tempID);

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

				renderImportAsset = false;
			}
			ImVec2 cancelButtonPos = ImVec2(300 + buttonSize.x / 2, 550);
			ImGui::SetCursorPos(cancelButtonPos);
			if (ImGui::Button("Cancel", buttonSize))
			{
				renderImportAsset = false;
			}

			ImGui::End();
		}
	}
}