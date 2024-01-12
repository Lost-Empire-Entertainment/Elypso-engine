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
#include "gui_console.hpp"
#include "render.hpp"
#include "timeManager.hpp"
#include "input.hpp"
#include "console.hpp"

#include <type_ptr.hpp>

using Graphics::GUI::GUIConsole;
using Graphics::Render;
using Core::TimeManager;
using Core::Input;
using Core::ConsoleManager;

namespace Graphics::GUI
{
	void GUIDebugMenu::RenderDebugMenu()
	{
		ImVec2 initialPos(5, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

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
		ImGui::Text("FOV: %.0f", Graphics::Render::fov);

		ImGui::Separator();

		ImGui::Text("Forwards: W");
		ImGui::Text("Backwards: S");
		ImGui::Text("Left: A");
		ImGui::Text("Right: D");
		ImGui::Text("Up: Space");
		ImGui::Text("Down: Left Control");
		ImGui::Text("Sprint: Left Shift");
		ImGui::Text("Toggle camera: Escape");
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
		//CAMERA CLIP RANGE
		//

		ImGui::Separator();

		ImGui::Text("Camera clip range");
		ImGui::Text("");

		ImGui::Text("Near clip");
		ImGui::SliderFloat("##nearclip", &Render::nearClip, 0.001f, 10.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera near clip range.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##nearclip"))
		{
			Render::nearClip = 0.001f;
		}

		ImGui::Text("Far clip");
		ImGui::SliderFloat("##farclip", &Render::farClip, 10.0f, 1000.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera far clip range.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##farclip"))
		{
			Render::farClip = 100.0f;
		}

		//
		//MOVE SPEED MULTIPLIER
		//

		ImGui::Separator();

		ImGui::Text("Move speed multiplier");
		ImGui::Text("");
		ImGui::SliderFloat("##movespeed", &Input::inputSettings.moveSpeedMultiplier, 0.1f, 10.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera move speed.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##movespeedmult"))
		{
			Input::inputSettings.moveSpeedMultiplier = 1.0f;
		}

		//
		//FOV
		//

		ImGui::Separator();

		ImGui::Text("FOV");
		ImGui::Text("");
		ImGui::SliderFloat("##fov", &Render::fov, 70.0f, 110.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera field of view.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##fov"))
		{
			Render::fov = 90.0f;
		}
	}
}