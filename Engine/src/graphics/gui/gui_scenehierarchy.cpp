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
#include "gui_scenehierarchy.hpp"
#include "gui.hpp"
#include "configFile.hpp"

using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;

namespace Graphics::GUI
{
	void GUISceneHierarchy::RenderSceneHierarchy()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		static bool renderSceneHierarchy;
		if (ConfigFileManager::valuesMap.size() > 0)
		{
			renderSceneHierarchy = stoi(ConfigFileManager::valuesMap["gui_sceneHierarchy"].GetValue());
		}

		if (renderSceneHierarchy
			&& ImGui::Begin("Scene hierarchy", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFileManager::valuesMap["gui_sceneHierarchy"].SetValue("0");
			}

			RenderSceneHierarchyContent();

			ImGui::End();
		}
	}

	void GUISceneHierarchy::RenderSceneHierarchyContent()
	{

	}
}