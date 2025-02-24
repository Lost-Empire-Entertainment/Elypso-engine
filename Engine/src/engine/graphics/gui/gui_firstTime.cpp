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
#include "gui_firstTime.hpp"
#include "configFile.hpp"
#include "gui_engine.hpp"

using EngineFile::ConfigFile;

namespace Graphics::GUI
{
	void GUIFirstTime::RenderFirstTime()
	{
		ImVec2 windowSize = ImVec2(600.0f, 600.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = EngineGUI::CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		bool renderFirstTime = stoi(ConfigFile::GetValue("gui_firstTime"));

		if (renderFirstTime
			&& ImGui::Begin("Welcome", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_firstTime", "0");
				ConfigFile::SaveConfigFile();
			}

			RenderFirstTimeContent();

			ImGui::End();
		}
	}

	void GUIFirstTime::RenderFirstTimeContent()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		const char* text = "Centered Text";

		ImVec2 textSize = ImGui::CalcTextSize(text);

		float posX = (windowSize.x - textSize.x) * 0.5f;
		float posY = (windowSize.y - textSize.y) * 0.5f;

		posX = ImClamp(posX, 50.0f, windowSize.x - textSize.x - 50.0f);
		posY = ImClamp(posY, 50.0f, windowSize.y - textSize.y - 50.0f);

		ImGui::SetCursorPos(ImVec2(posX, posY));

		ImGui::Text("%s", text);
	}
}
#endif