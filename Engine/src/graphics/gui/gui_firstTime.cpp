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
#include "gui_firstTime.hpp"
#include "configFile.hpp"

using EngineFile::ConfigFile;

namespace Graphics::GUI
{
	void GUIFirstTime::RenderFirstTime()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

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
			}

			RenderFirstTimeContent();

			ImGui::End();
		}
	}

	void GUIFirstTime::RenderFirstTimeContent()
	{
		ImGui::Text("1234");
	}
}