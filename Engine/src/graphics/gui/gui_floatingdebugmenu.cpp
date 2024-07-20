//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <type_ptr.hpp>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_floatingdebugmenu.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "input.hpp"
#include "timeManager.hpp"
#include "sceneFile.hpp"
#include "grid.hpp"
#include "configFile.hpp"
#include "stringUtils.hpp"

using Graphics::Render;
using Core::Input;
using Core::TimeManager;
using EngineFile::SceneFile;
using Graphics::Grid;
using EngineFile::ConfigFile;
using Utils::String;

namespace Graphics::GUI
{
	void GUIFloatingDebugMenu::RenderFloatingDebugMenu()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoDocking;

		if (ImGui::Begin("Floating debug menu", NULL, windowFlags))
		{
			FloatingDebugMenuContent();

			ImGui::End();
		}
	}

	void GUIFloatingDebugMenu::FloatingDebugMenuContent()
	{
		//
		// GENERAL
		//

		ImGui::Text("FPS: %.2f", TimeManager::displayedFPS);

		ImGui::Text(
			"Position: %.2f, %.2f, %.2f",
			Render::camera.GetCameraPosition().x,
			Render::camera.GetCameraPosition().y,
			Render::camera.GetCameraPosition().z);
		ImGui::Text(
			"Angle: %.2f, %.2f, %.2f",
			Render::camera.GetCameraRotation().x,
			Render::camera.GetCameraRotation().y,
			Render::camera.GetCameraRotation().z);

		ImGui::Text("Current axis: %s", Input::axis.c_str());
		ImGui::Text("Current tool: %s", string(magic_enum::enum_name(Input::objectAction)).c_str());

		//
		// FOV
		//

		ImGui::Separator();

		ImGui::Text("FOV");
		float fov = stof(ConfigFile::GetValue("camera_fov"));
		if (ImGui::DragFloat("##fov", &fov, 0.1f, 70.0f, 110.0f))
		{
			ConfigFile::SetValue("camera_fov", to_string(fov));
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##fov"))
		{
			ConfigFile::SetValue("camera_fov", "90.0");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		//
		// CAMERA
		//

		ImGui::Separator();

		ImGui::Text("Camera near clip");
		float nearClip = stof(ConfigFile::GetValue("camera_nearClip"));
		float farClip = stof(ConfigFile::GetValue("camera_farClip"));
		if (ImGui::DragFloat("##camNearClip", &nearClip, 0.1f, 0.001f, farClip - 0.001f))
		{
			ConfigFile::SetValue("camera_nearClip", to_string(nearClip));
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##camNearClip"))
		{
			ConfigFile::SetValue("camera_nearClip", "0.001");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Camera far clip");
		if (ImGui::DragFloat("##camFarClip", &farClip, 0.1f, nearClip + 0.001f, 10000))
		{
			ConfigFile::SetValue("camera_farClip", to_string(farClip));
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##camFarClip"))
		{
			ConfigFile::SetValue("camera_farClip", "100.0");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Camera move speed multiplier");
		float moveSpeed = stof(ConfigFile::GetValue("camera_speedMultiplier"));
		if (ImGui::DragFloat("##camMoveSpeed", &moveSpeed, 0.1f, 0.1f, 100.0))
		{
			ConfigFile::SetValue("camera_speedMultiplier", to_string(moveSpeed));
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##camMoveSpeed"))
		{
			ConfigFile::SetValue("camera_speedMultiplier", "1.0");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		//
		// GRID
		//

		ImGui::Separator();

		ImGui::Text("Grid color");
		string gridColorString = ConfigFile::GetValue("grid_color");
		vector<string> gridColorSplit = String::Split(gridColorString, ',');
		vec3 gridColor = vec3(
			stof(gridColorSplit[0]), 
			stof(gridColorSplit[1]),
			stof(gridColorSplit[2]));
		if (ImGui::ColorEdit3("##gridColor", value_ptr(gridColor)))
		{
			string finalString = 
				to_string(gridColor[0]) + "," +
				to_string(gridColor[1]) + "," +
				to_string(gridColor[2]);

			ConfigFile::SetValue("grid_color", finalString);
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##gridColor"))
		{
			ConfigFile::SetValue("grid_color", "0.4, 0.4, 0.4");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Grid transparency");
		float gridTransparency = stof(ConfigFile::GetValue("grid_transparency"));
		if (ImGui::DragFloat("##gridTransparency", &gridTransparency, 0.001f, 0.0f, 1.0f))
		{
			ConfigFile::SetValue("grid_transparency", to_string(gridTransparency));
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##gridTransparency"))
		{
			ConfigFile::SetValue("grid_transparency", "0.25");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
	}
}