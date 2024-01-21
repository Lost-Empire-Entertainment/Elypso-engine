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
		// CAMERA CLIP RANGE
		//

		ImGui::Separator();

		ImGui::Text("Camera clip range");
		ImGui::Text("");

		ImGui::Text("Near clip");
		string nearClipText = to_string(Render::nearClip);
		strcpy_s(inputTextBuffer_camNearClip, bufferSize, nearClipText.c_str());
		if (ImGui::InputText("##camNearClip", inputTextBuffer_camNearClip, bufferSize))
		{
			try
			{
				camNearClip = stof(inputTextBuffer_camNearClip);

				if (camNearClip < 0.0001f) camNearClip = 0.0001f;
				if (camNearClip > camFarClip - 0.1f)
				{
					camNearClip = camFarClip - 0.1f;
				}

				Render::nearClip = camNearClip;
			}
			catch (...)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::EXCEPTION,
					"Error: Debug menu camera near clip cannot be empty!\n");
			}
		}

		ImGui::Text("Far clip");
		string farClipText = to_string(Render::farClip);
		strcpy_s(inputTextBuffer_camFarClip, bufferSize, farClipText.c_str());
		if (ImGui::InputText("##camFarClip", inputTextBuffer_camFarClip, bufferSize))
		{
			try
			{
				camFarClip = stof(inputTextBuffer_camFarClip);

				if (camFarClip < 0.1f) camFarClip = 0.1f;
				if (camFarClip < camNearClip + 0.1f)
				{
					camFarClip = camNearClip + 0.1f;
				}
				if (camFarClip > 10000.0f) camFarClip = 10000.0f; //HARD-CAPPED TO 100000 TO SAVE PERFORMANCE

				Render::farClip = camFarClip;
			}
			catch (...)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::EXCEPTION,
					"Error: Debug menu camera far clip cannot be empty!\n");
			}
		}

		//
		// MOVE SPEED MULTIPLIER
		//

		ImGui::Separator();

		ImGui::Text("Move speed multiplier");
		string camMoveSpeedText = to_string(Input::inputSettings.moveSpeedMultiplier);
		strcpy_s(inputTextBuffer_camMoveSpeedMult, bufferSize, camMoveSpeedText.c_str());
		if (ImGui::InputText("##camMoveSpeed", inputTextBuffer_camMoveSpeedMult, bufferSize))
		{
			try
			{
				camMovespeed = stof(inputTextBuffer_camMoveSpeedMult);

				if (camMovespeed < 0) camMovespeed = 0;
				if (camMovespeed > 100.0f) camMovespeed = 100.0f;

				Input::inputSettings.moveSpeedMultiplier = camMovespeed;
			}
			catch (...)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::EXCEPTION,
					"Error: Debug menu camera movement speed multiplier cannot be empty!\n");
			}
		}

		//
		// FOV
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

		//
		// GRID
		//

		ImGui::Separator();

		ImGui::Text("Grid color");
		ImGui::ColorEdit3("##gridColor", value_ptr(Grid::color));

		ImGui::Text("Grid transparency");
		ImGui::SliderFloat("##gridTransparency", &Grid::transparency, 0.0f, 1.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##gridTransparency"))
		{
			Grid::transparency = 0.1f;
		}
	}
}