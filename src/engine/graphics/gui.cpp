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

		if (Input::printIMGUIToConsole)
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
		GUI::RDM_DebugKeys();

		if (Input::PrintIMGUIDebugToConsole
			&& ImGui::IsItemHovered())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::IMGUI,
				Type::DEBUG,
				"Mouse is hovering over the debug menu!\n");
		}

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
		GUI::RS_MouseSpeed();
		GUI::RS_MoveSpeedMultiplier();
		GUI::RS_FOV();

		if (Input::PrintIMGUIDebugToConsole
			&& ImGui::IsItemHovered())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::IMGUI,
				Type::DEBUG,
				"Mouse is hovering over the sliders menu!\n");
		}

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
		string fullScreenText = (Render::enableFullscreen) ?
			"(true)" :
			"(false)";
		ImGui::Text(
			"Toggle fullscreen: %s %s", 
			string(magic_enum::enum_name(Input::key[Input::Action::ToggleFullscreen])), 
			fullScreenText);
		string vsyncText = (Render::useMonitorRefreshRate) ?
			"(true)" :
			"(false)";
		ImGui::Text(
			"Toggle VSync: %s %s", 
			string(magic_enum::enum_name(Input::key[Input::Action::ToggleVSYNC])), 
			vsyncText);
		string mouseFocusText = (Input::mouseFocused) ?
			"(true)" :
			"(false)";
		ImGui::Text("Toggle focus: Escape %s", mouseFocusText);
	}
	void GUI::RDM_DebugKeys()
	{
		ImGui::Separator();

		ImGui::Text("Debug keys");
		ImGui::Text("");
		string fpsText = (Input::printFPSToConsole) ?
			"(true)" :
			"(false)";
		ImGui::Text(
			"FPS messages: %s %s", 
			string(magic_enum::enum_name(Input::key[Input::Action::PrintFPSDebugToConsole])), 
			fpsText);
		string imguiText = (Input::printIMGUIToConsole) ?
			"(true)" :
			"(false)";
		ImGui::Text(
			"ImGui messages: %s %s", 
			string(magic_enum::enum_name(Input::key[Input::Action::PrintIMGUIDebugToConsole])), 
			imguiText);
		string inputText = (Input::printInputToConsole) ?
			"(true)" :
			"(false)";
		ImGui::Text(
			"Input messages: %s %s", 
			string(magic_enum::enum_name(Input::key[Input::Action::PrintInputDebugToConsole])), 
			inputText);
	}

	void GUI::RS_CameraClipRange()
	{
		ImGui::Text("Camera clip range");
		ImGui::Text("");

		ImGui::Text("Near clip");
		ImGui::SliderFloat("", &Render::nearClip, 0.001f, 1.0f);

		if (Input::printIMGUIToConsole
			&& ImGui::IsItemHovered())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::IMGUI,
				Type::DEBUG,
				"Hovering over near clip slider!");
		}

		ImGui::Text("Far clip");
		ImGui::SliderFloat("", &Render::farClip, 1.0f, 100.0f);

		if (Input::printIMGUIToConsole
			&& ImGui::IsItemHovered())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::IMGUI,
				Type::DEBUG,
				"Hovering over far clip slider!");
		}
	}
	void GUI::RS_MouseSpeed()
	{
		ImGui::Separator();

		ImGui::Text("Mouse speed multiplier");
		ImGui::Text("");
		ImGui::SliderFloat("", &Input::mouseSpeedMultiplier, 0.1f, 10.0f);

		if (Input::printIMGUIToConsole
			&& ImGui::IsItemHovered())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::IMGUI,
				Type::DEBUG,
				"Hovering over mouse speed multiplier slider!");
		}
	}
	void GUI::RS_MoveSpeedMultiplier()
	{
		ImGui::Separator();

		ImGui::Text("Move speed multiplier");
		ImGui::Text("");
		ImGui::SliderFloat("", &Input::moveSpeedMultiplier, 0.1f, 10.0f);

		if (Input::printIMGUIToConsole
			&& ImGui::IsItemHovered())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::IMGUI,
				Type::DEBUG,
				"Hovering over move speed multiplier slider!");
		}
	}
	void GUI::RS_FOV()
	{
		ImGui::Separator();

		ImGui::Text("FOV");
		ImGui::Text("");
		ImGui::SliderFloat("", &Render::fov, 70.0f, 110.0f);

		if (Input::printIMGUIToConsole
			&& ImGui::IsItemHovered())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::IMGUI,
				Type::DEBUG,
				"Hovering over fov slider!");
		}
	}

	void GUI::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}