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
#include "gui_projecthierarchy.hpp"
#include "gui.hpp"
#include "configFile.hpp"

using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;

namespace Graphics::GUI
{
	void GUIProjectHierarchy::RenderProjectHierarchy()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		static bool renderProjectHierarchy;
		if (ConfigFileManager::valuesMap.size() > 0)
		{
			renderProjectHierarchy = stoi(ConfigFileManager::valuesMap["gui_projectHierarchy"].GetValue());
		}

		if (renderProjectHierarchy
			&& ImGui::Begin("Project hierarchy", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFileManager::valuesMap["gui_projectHierarchy"].SetValue("0");
			}

			ImGui::End();
		}
	}
}