//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_credits.hpp"
#include "gui_engine.hpp"

namespace Graphics::GUI
{
	void GUICredits::RenderCreditsWindow()
	{
		ImVec2 windowSize = ImVec2(600.0f, 600.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

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

			ImGui::Text(" ");
			ImGui::Separator();
			ImGui::Text("Testers");
			ImGui::Separator();

			ImGui::Text("tkpmonke");
		}
		ImGui::EndChild();
	}
}
#endif