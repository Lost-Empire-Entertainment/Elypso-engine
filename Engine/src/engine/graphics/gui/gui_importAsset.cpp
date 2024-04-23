//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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

using Graphics::Shape::Model;
using EngineFile::SceneFile;
using Graphics::Render;

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
			if (ImGui::Button("Import"))
			{
				Model::targetModel = targetModelPath;
				Model::Initialize();

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

				renderImportAsset = false;
			}
			if (ImGui::Button("Cancel"))
			{
				renderImportAsset = false;
			}

			ImGui::End();
		}
	}
}