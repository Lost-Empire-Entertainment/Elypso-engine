//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "gui_game.hpp"

//game
#include "g_ui.hpp"

//engine
using Graphics::GUI::GameGUI;

namespace GameTemplate
{
	void G_UI::AddWindowsToList()
	{
		//GameGUI::imguiWindows.push_back(TestWindow);
	}

	void G_UI::TestWindow()
	{
		ImVec2 windowSize = ImVec2(600.0f, 600.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = GameGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (ImGui::Begin("Test1234", NULL, windowFlags))
		{
			ImGui::Text("Test");

			ImGui::End();
		}
	}
}