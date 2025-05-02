//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "gui_game.hpp"

//game
#include "g_ui.hpp"

using Graphics::GUI::GameGUI;

namespace GameTemplate
{
	void G_UI::AddWindowsToList()
	{
		//GameGUI::imguiWindows.push_back(DebugWindow);
	}

	void G_UI::DebugWindow()
	{
		ImVec2 windowSize = ImVec2(400.0f, 400.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

		ImVec2 windowPos = GameGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings;

		if (ImGui::Begin("Debug Window", NULL, windowFlags))
		{
			ImGui::Text("test");
			ImGui::End();
		}
	}
}