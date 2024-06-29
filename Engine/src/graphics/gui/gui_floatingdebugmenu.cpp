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

using Graphics::Render;
using Core::Input;
using Core::TimeManager;
using EngineFile::SceneFile;
using Graphics::Grid;
using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;

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
		float fov = stof(ConfigFileManager::valuesMap["camera_fov"].GetValue());
		if (ImGui::DragFloat("##fov", &fov, 0.1f, 70.0f, 110.0f))
		{
			ConfigFileManager::valuesMap["camera_fov"].SetValue(to_string(fov));
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##fov"))
		{
			ConfigFileManager::valuesMap["camera_fov"].SetValue("90.0");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		//
		// CAMERA
		//

		ImGui::Separator();

		ImGui::Text("Camera near clip");
		float nearClip = stof(ConfigFileManager::valuesMap["camera_nearClip"].GetValue());
		float farClip = stof(ConfigFileManager::valuesMap["camera_farClip"].GetValue());
		if (ImGui::DragFloat("##camNearClip", &nearClip, 0.1f, 0.001f, farClip - 0.001f))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Camera far clip");
		if (ImGui::DragFloat("##camFarClip", &farClip, 0.1f, nearClip + 0.001f, 10000))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Camera move speed multiplier");
		if (ImGui::DragFloat("##camMoveSpeed", &Input::moveSpeedMultiplier, 0.1f, 0.0f, 100.0f))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##camMoveSpeed"))
		{
			Input::moveSpeedMultiplier = 1.0f;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		//
		// GRID
		//

		ImGui::Separator();

		ImGui::Text("Grid color");
		if (ImGui::ColorEdit3("##gridColor", value_ptr(Grid::color)))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Grid transparency");
		if (ImGui::DragFloat("##gridTransparency", &Grid::transparency, 0.001f, 0.0f, 1.0f))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##gridTransparency"))
		{
			Grid::transparency = 0.25f;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
	}
}