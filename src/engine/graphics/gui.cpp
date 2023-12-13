//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//external
#include "glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "magic_enum.hpp"

//engine
#include "console.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "input.hpp"
#include "render.hpp"
#include "stringUtils.hpp"
#include "timeManager.hpp"

#include <sstream>
#include <string>

using namespace std;
using namespace Core;
using namespace Utils;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics
{
	GUI& GUI::GetInstance()
	{
		static GUI instance;
		return instance;
	}

	void GUI::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		ImGui::StyleColorsDark();
	}

	int GUI::GetScreenWidth()
	{
		int width, height;
		glfwGetFramebufferSize(Render::window, &width, &height);
		return width;
	}

	int GUI::GetScreenHeight()
	{
		int width, height;
		glfwGetFramebufferSize(Render::window, &width, &height);
		return height;
	}

	float GUI::GetScreenRefreshRate()
	{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

		return static_cast<float>(videoMode->refreshRate);
	}

	void GUI::Render()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		GUI::RenderDebugMenu();
		GUI::RenderSlider();

		if (Input::inputSettings.printIMGUIToConsole)
		{
			/*
			ImVec2 mousePos = ImGui::GetMousePos();
			ostringstream messageStream;
			messageStream <<
				"Mouse Position: " <<
				fixed << 
				setprecision(2) <<
				mousePos.x << "," <<
				mousePos.y << "\n";
			ConsoleManager::WriteConsoleMessage(
				Caller::IMGUI, 
				Type::DEBUG, 
				messageStream.str());
			
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::IMGUI, 
					Type::DEBUG, 
					"W key pressed!\n");
			}

			if (ImGui::IsMousePosValid())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::IMGUI,
					Type::DEBUG,
					"Mouse position is valid!\n");
			}
			else
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::IMGUI,
					Type::DEBUG,
					"Mouse position is not valid!\n");
			}
			*/

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::IMGUI, 
					Type::DEBUG, 
					"Left mouse button clicked!\n");
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GUI::RenderDebugMenu()
	{
		//docked and not movable
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings;

		ImVec4 bgColor = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);

		//window initial size
		ImVec2 initialSize(350, 700);

		//start a new window with specified flags and size
		ImGui::SetNextWindowSize(initialSize, ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGui::Begin("Debug menu", nullptr, windowFlags);

		//set font size
		ImGui::SetWindowFontScale(1.5);

		GUI::RDM_Info();
		GUI::RDM_GeneralKeys();
		GUI::RDM_DebugButtons();

		ImGui::End();
	}

	// render sliders menu at the top right corner like RenderDebugMenu()
	void GUI::RenderSlider()
	{
		//docked and not movable
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings;

		ImVec4 bgColor = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);

		ImVec2 initialSize(350, 700);

		ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

		ImVec2 windowPos(viewportSize.x - initialSize.x, 0);

		ImGui::SetNextWindowSize(initialSize, ImGuiCond_Once);
		ImGui::SetNextWindowPos(windowPos);

		ImGui::Begin("Sliders menu", nullptr, windowFlags);

		//set font size
		ImGui::SetWindowFontScale(1.5);

		GUI::RS_CameraClipRange();
		GUI::RS_MoveSpeedMultiplier();
		GUI::RS_FOV();

		ImGui::End();
	}

	void GUI::RDM_Info()
	{
		ImGui::Text("Version: %s", Engine::version);
		ImGui::Text("FPS: %.2f", TimeManager::displayedFPS);
		ImGui::Text(
			"Position: %.2f, %.2f, %.2f",
			Render::cameraPos.x,
			Render::cameraPos.y,
			Render::cameraPos.z);
		ImGui::Text(
			"Angle: %.2f, %.2f, %.2f",
			Render::camera.GetCameraRotation().x,
			Render::camera.GetCameraRotation().y,
			Render::camera.GetCameraRotation().z);
		ImGui::Text("FOV: %.0f", Graphics::Render::fov);
	}
	void GUI::RDM_GeneralKeys()
	{
		ImGui::Separator();

		ImGui::Text("General keys");
		ImGui::Text("");
		ImGui::Text("Forwards: %s", string(magic_enum::enum_name(Input::key[Input::Action::CameraForwards])));
		ImGui::Text("Backwards: %s", string(magic_enum::enum_name(Input::key[Input::Action::CameraBackwards])));
		ImGui::Text("Left: %s", string(magic_enum::enum_name(Input::key[Input::Action::CameraLeft])));
		ImGui::Text("Right: %s", string(magic_enum::enum_name(Input::key[Input::Action::CameraRight])));
		ImGui::Text("Up: %s", string(magic_enum::enum_name(Input::key[Input::Action::CameraUp])));
		ImGui::Text("Down: %s", string(magic_enum::enum_name(Input::key[Input::Action::CameraDown])));
		ImGui::Text("Sprint: %s", string(magic_enum::enum_name(Input::key[Input::Action::CameraSprint])));
		string cameraEnabledText = (Input::inputSettings.cameraEnabled) ?
			"(true)" :
			"(false)";
		ImGui::Text("Toggle camera: Escape %s", cameraEnabledText);
	}
	void GUI::RDM_DebugButtons()
	{
		ImGui::Separator();

		ImGui::Text("Debug buttons");
		ImGui::Text("");

		if (ImGui::Button(Render::enableFullscreen ?
			"Disable fullscreen" :
			"Enable fullscreen"))
		{
			Render::enableFullscreen = !Render::enableFullscreen;
			Render::ToggleFullscreenMode(Render::window, Render::enableFullscreen ? false : true);
		}
		if (ImGui::Button(Render::useMonitorRefreshRate ?
			"Disable VSync" :
			"Enable VSync"))
		{
			Render::useMonitorRefreshRate = !Render::useMonitorRefreshRate;
			glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);
		}

		ImGui::Text("");

		if (ImGui::Button(Input::inputSettings.printFPSToConsole ?
			"Disable FPS messages" :
			"Enable FPS messages"))
		{
			Input::inputSettings.printFPSToConsole = !Input::inputSettings.printFPSToConsole;
		}
		if (ImGui::Button(Input::inputSettings.printIMGUIToConsole ?
			"Disable ImGui messages" :
			"Enable ImGui messages"))
		{
			Input::inputSettings.printIMGUIToConsole = !Input::inputSettings.printIMGUIToConsole;
		}
		if (ImGui::Button(Input::inputSettings.printInputToConsole ?
			"Disable input messages" :
			"Enable input messages"))
		{
			Input::inputSettings.printInputToConsole = !Input::inputSettings.printInputToConsole;
		}
	}

	void GUI::RS_CameraClipRange()
	{
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
		ImGui::SliderFloat("##farclip", &Render::farClip, 10.0f, 100.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust camera far clip range.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##farclip"))
		{
			Render::farClip = 100.0f;
		}
	}
	void GUI::RS_MoveSpeedMultiplier()
	{
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
	}
	void GUI::RS_FOV()
	{
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

	void GUI::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}