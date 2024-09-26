//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//game
#include "gui.hpp"
#include "gui_game.hpp"

using Graphics::GUI::GameGUI;

namespace TheGameGUI
{
	void GUICore::AddWindowsToList()
	{
		GameGUI::imguiWindows.push_back(TestWindow);
	}

	void GUICore::TestWindow()
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