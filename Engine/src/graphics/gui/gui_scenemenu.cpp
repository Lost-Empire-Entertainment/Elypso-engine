//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <type_ptr.hpp>
#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_scenemenu.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "sceneFile.hpp"
#include "core.hpp"
#include "fileexplorer.hpp"
#include "configFile.hpp"

using std::cout;

using Graphics::Render;
using EngineFile::SceneFile;
using Core::Engine;
using EngineFile::FileExplorer;
using EngineFile::ConfigFile;

namespace Graphics::GUI
{
	void GUISceneMenu::RenderSceneMenu()
	{
		ImVec2 windowSize = ImVec2(400.0f, 400.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		bool renderSceneMenu = stoi(ConfigFile::GetValue("gui_sceneMenu"));

		if (renderSceneMenu
			&& ImGui::Begin("Scene menu", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_sceneMenu", "0");
			}

			SceneMenuContent();

			ImGui::End();
		}
	}

	void GUISceneMenu::SceneMenuContent()
	{
		//
		// BACKGROUND
		//

		ImGui::Separator();

		ImGui::Text("Background color");
		if (ImGui::ColorEdit3("##bgrdiff", value_ptr(Render::backgroundColor)))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		//
		// DIRECTIONAL LIGHT
		//

		ImGui::Separator();

		ImGui::Text("Directional light direction");
		ImGui::Text("x  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::DragFloat("##dirX", &Render::directionalDirection.x, 0.1f, -360.0f, 360.0f))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		ImGui::Text("  y  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::DragFloat("##dirY", &Render::directionalDirection.y, 0.1f, -360.0f, 360.0f))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		ImGui::Text("  z  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::DragFloat("##dirZ", &Render::directionalDirection.z, 0.1f, -360.0f, 360.0f))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Directional light diffuse");
		if (ImGui::ColorEdit3("##dirdiff", value_ptr(Render::directionalDiffuse)))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Directional light intensity");
		if (ImGui::DragFloat("##dirint", &Render::directionalIntensity, 0.001f, 0.0f, 5.0f))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##dirint"))
		{
			Render::directionalIntensity = 1.0f;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
	}
}