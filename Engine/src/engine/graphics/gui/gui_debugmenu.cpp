//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_debugmenu.hpp"
#include "gui.hpp"
#include "gui_console.hpp"
#include "render.hpp"
#include "input.hpp"
#include "console.hpp"
#include "timeManager.hpp"
#include "grid.hpp"

#include <type_ptr.hpp>
#include <cmath>

using std::to_string;
using std::stof;
using std::round;

using Core::TimeManager;
using Graphics::GUI::GUIConsole;
using Graphics::Render;
using Graphics::Grid;
using Core::Input;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::GUI
{
	void GUIDebugMenu::RenderDebugMenu()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (renderDebugMenu
			&& ImGui::Begin("Debug menu", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderDebugMenu = false;
			}

			if (ImGui::BeginTabBar("Debug"))
			{
				if (ImGui::BeginTabItem("Debug info"))
				{
					RD_DebugMenuInfo();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Debug interactions"))
				{
					RD_Interactions();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}
	void GUIDebugMenu::RD_DebugMenuInfo()
	{
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
		ImGui::Text("FOV: %.0f", Input::inputSettings.fov);

		ImGui::Separator();

		ImGui::Text("Forwards: W");
		ImGui::Text("Backwards: S");
		ImGui::Text("Left: A");
		ImGui::Text("Right: D");
		ImGui::Text("Up: Space");
		ImGui::Text("Down: Left Control");
		ImGui::Text("Sprint: Left Shift");
		ImGui::Text("Toggle camera: Escape");
		ImGui::Text("Select GameObject: Left Mouse Button");
		ImGui::Text("Delete selected GameObject: Delete");
	}
	void GUIDebugMenu::RD_Interactions()
	{
		ImGui::Text("Debug buttons");
		ImGui::Text("");

		ImGui::Text("Enable VSync");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		if (ImGui::Checkbox("##vsync", &Render::useMonitorRefreshRate))
		{
			glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);
		}

		ImGui::Text("Enable FPS messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##fpsmsg", &Input::inputSettings.printFPSToConsole);

		ImGui::Text("Enable ImGui messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##imguimsg", &Input::inputSettings.printIMGUIToConsole);

		ImGui::Text("Enable input messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##inputmsg", &Input::inputSettings.printInputToConsole);

		ImGui::Text("Enable select ray direction messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##raymsg", &Input::inputSettings.printSelectRayDirectionToConsole);

		ImGui::Text("Enable console scroll to bottom");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##consolescroll", &GUIConsole::allowScrollToBottom);

		ImGui::Text("Enable console debug messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		ImGui::Checkbox("##consoledebugmsg", &ConsoleManager::sendDebugMessages);

		//
		// CAMERA CLIP RANGE
		//

		ImGui::Separator();

		ImGui::Text("Camera clip range");
		ImGui::Text("");

		ImGui::Text("Near clip");
		ImGui::DragFloat("##camNearClip", &Input::inputSettings.nearClip, 0.1f, 0.001f, Input::inputSettings.farClip - 0.001f);

		ImGui::Text("Far clip");
		ImGui::DragFloat("##camFarClip", &Input::inputSettings.farClip, 0.1f, Input::inputSettings.nearClip + 0.001f, 10000);

		//
		// MOVE SPEED MULTIPLIER
		//

		ImGui::Separator();

		ImGui::Text("Move speed multiplier");
		ImGui::DragFloat("##camMoveSpeed", &Input::inputSettings.moveSpeedMultiplier, 0.1f, 0.0f, 100.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##camMoveSpeed"))
		{
			Input::inputSettings.moveSpeedMultiplier = 1.0f;
		}

		//
		// FOV
		//

		ImGui::Separator();

		ImGui::Text("FOV");
		ImGui::DragFloat("##fov", &Input::inputSettings.fov, 0.1f, 70.0f, 110.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##fov"))
		{
			Input::inputSettings.fov = 90.0f;
		}

		//
		// GRID
		//

		ImGui::Separator();

		ImGui::Text("Grid color");
		ImGui::ColorEdit3("##gridColor", value_ptr(Grid::color));

		ImGui::Text("Grid transparency");
		ImGui::DragFloat("##gridTransparency", &Grid::transparency, 0.001f, 0.0f, 1.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##gridTransparency"))
		{
			Grid::transparency = 0.25f;
		}

		//
		// BACKGROUND
		//

		ImGui::Separator();

		ImGui::Text("Background color");
		ImGui::ColorEdit3("##bgrdiff", value_ptr(Render::backgroundColor));

		//
		// DIRECTIONAL LIGHT
		//

		ImGui::Separator();

		ImGui::Text("Directional light direction");
		ImGui::Text("x  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::DragFloat("##dirX", &Render::directionalDirection.x, 0.1f, -360.0f, 360.0f);
		ImGui::SameLine();
		ImGui::Text("  y  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::DragFloat("##dirY", &Render::directionalDirection.y, 0.1f, -360.0f, 360.0f);
		ImGui::SameLine();
		ImGui::Text("  z  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::DragFloat("##dirZ", &Render::directionalDirection.z, 0.1f, -360.0f, 360.0f);

		ImGui::Text("Directional light diffuse");
		ImGui::ColorEdit3("##dirdiff", value_ptr(Render::directionalDiffuse));

		ImGui::Text("Directional light intensity");
		ImGui::DragFloat("##dirint", &Render::directionalIntensity, 0.001f, 0.0f, 5.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##dirint"))
		{
			Render::directionalIntensity = 1.0f;
		}
	}
}