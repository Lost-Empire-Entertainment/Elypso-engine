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

//hub
#include "removeproject.hpp"
#include "gui.hpp"

using std::cout;

namespace Graphics::GUI
{
	void RemoveProject::RenderRemoveProjectWindow()
	{
		ImVec2 size = ImVec2(300.0f, 200.0f);
		ImVec2 windowPos = GUI_Hub::CenterWindow(size);

		ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoTitleBar;

		if (renderRemoveProjectWindow
			&& ImGui::Begin("Remove project", NULL, windowFlags))
		{
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
			{
				ImGui::SetWindowFocus();
			}

			RenderRemoveProjectContent();

			ImGui::End();
		}
	}

	void RemoveProject::RenderRemoveProjectContent()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		string text = "Remove " + confirmFileName + "?";
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;
		float offsetX = (windowSize.x - textWidth) * 0.5f;
		if (offsetX > 0.0f) ImGui::SetCursorPos(ImVec2(offsetX, 30));

		ImGui::Text("%s", text.c_str());

		ImVec2 buttonSize = ImVec2(100, 30);

		ImVec2 createButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2 + 25,
			ImGui::GetWindowSize().y - 60);
		ImGui::SetCursorPos(createButtonPos);
		if (ImGui::Button("Yes", buttonSize))
		{
			GUI_Hub::RemoveProject(confirmFilePath);
			renderRemoveProjectWindow = false;
		}

		ImVec2 cancelButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 + buttonSize.x / 2 - 25,
			ImGui::GetWindowSize().y - 60);
		ImGui::SetCursorPos(cancelButtonPos);
		if (ImGui::Button("No", buttonSize))
		{
			cout << "Cancelled project removal.\n\n";
			renderRemoveProjectWindow = false;
		}
	}
}