//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <type_ptr.hpp>

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
using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;

namespace Graphics::GUI
{
	GUISceneMenu sceneMenu;

	void GUISceneMenu::RenderSceneMenu()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		static bool renderSceneMenu;
		if (ConfigFileManager::valuesMap.size() > 0)
		{
			renderSceneMenu = stoi(ConfigFileManager::valuesMap["gui_sceneMenuWindow"].GetValue());
		}

		if (renderSceneMenu
			&& ImGui::Begin("Scene menu", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderSceneMenu = false;
			}

			sceneMenu.SceneMenuContent();

			ImGui::End();
		}
	}

	void GUISceneMenu::SceneMenuContent()
	{
		ImGui::Text("Enable VSync");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		if (ImGui::Checkbox("##vsync", &Render::useMonitorRefreshRate))
		{
			glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		if (ImGui::IsItemHovered())
		{
			string hint = "Toggles between 60 and uncapped framerate without affecting deltatime.";
			ImGui::SetTooltip(hint.c_str());
		}

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