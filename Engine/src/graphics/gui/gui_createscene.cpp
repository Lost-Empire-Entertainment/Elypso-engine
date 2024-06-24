//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_createscene.hpp"
#include "gui.hpp"

using std::cout;

namespace Graphics::GUI
{
	void GUICreateScene::RenderCreateSceneWindow()
	{
		ImVec2 minSize = ImVec2(600, 600);
		ImVec2 pos = ImVec2(400, 400);
		ImGui::SetNextWindowSizeConstraints(minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (renderCreateSceneWindow
			&& ImGui::Begin("Create scene", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderCreateSceneWindow = false;
			}

			RenderCreateSceneWindowContent();

			ImGui::End();
		}
	}

	void GUICreateScene::RenderCreateSceneWindowContent()
	{
		ImVec2 buttonSize = ImVec2(100, 30);

		ImVec2 importButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(importButtonPos);
		if (ImGui::Button("Create", buttonSize))
		{
			cout << "Created scene...\n";
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