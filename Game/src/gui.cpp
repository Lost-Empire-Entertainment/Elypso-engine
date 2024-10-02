//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "gui_game.hpp"

//game
#include "gui.hpp"

//engine
using Graphics::GUI::GameGUI;

namespace TheGameGUI
{
	void GUI::AddWindowsToList()
	{
		//GameGUI::imguiWindows.push_back(TestWindow);
	}

	void GUI::TestWindow()
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