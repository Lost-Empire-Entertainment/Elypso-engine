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
#include "gui_credits.hpp"
#include "gui.hpp"

namespace Graphics::GUI
{
	void GUICredits::RenderCreditsWindow()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (renderCreditsWindow
			&& ImGui::Begin("Credits", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderCreditsWindow = false;
			}

			RenderCreditsContent();

			ImGui::End();
		}
	}

	void GUICredits::RenderCreditsContent()
	{
		if (ImGui::BeginChild("##credits"))
		{
			ImGui::Separator();
			ImGui::Text("Developers");
			ImGui::Separator();

			ImGui::Text("Lead developer: greenlaser");

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("Testers");
			ImGui::Separator();

			ImGui::Text("None at the moment...");
		}
		ImGui::EndChild();
	}
}