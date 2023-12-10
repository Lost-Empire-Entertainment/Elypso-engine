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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "magic_enum.hpp"

//engine
#include "core.h"
#include "timeManager.h"
#include "gui.h"
#include "render.h"
#include "input.h"

#include <string>

using namespace std;
using namespace Core;

namespace Graphics
{
	GUI& GUI::GetInstance()
	{
		static GUI instance;
		return instance;
	}

	void GUI::Initialize()
	{
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
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
		ImVec2 initialSize(350, 450);

		//start a new window with specified flags and size
		ImGui::SetNextWindowSize(initialSize, ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGui::Begin("Debug menu", nullptr, windowFlags);

		//set font size
		ImGui::SetWindowFontScale(1.5);

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
		ImGui::Text("FOV: %.2f", Graphics::Render::fov);

		ImGui::Text("");

		ImGui::Text("Forwards: %s", string(magic_enum::enum_name(Input::Key::W)));
		ImGui::Text("Backwards: %s", string(magic_enum::enum_name(Input::Key::S)));
		ImGui::Text("Left: %s", string(magic_enum::enum_name(Input::Key::A)));
		ImGui::Text("Right: %s", string(magic_enum::enum_name(Input::Key::D)));
		ImGui::Text("Up: %s", string(magic_enum::enum_name(Input::Key::Space)));
		ImGui::Text("Down: %s", string(magic_enum::enum_name(Input::Key::Left_control)));
		ImGui::Text("Sprint: %s", string(magic_enum::enum_name(Input::Key::Left_shift)));
		ImGui::Text("Change FOV: scrollwheel");
		string fullScreenText = (Render::enableFullscreen) ?
			"(enabled)" :
			"(disabled)";
		ImGui::Text("Toggle fullscreen: %s, %s", string(magic_enum::enum_name(Input::Key::Z)), fullScreenText);
		string vsyncText = (Render::useMonitorRefreshRate) ?
			"(enabled)" :
			"(disabled)";
		ImGui::Text("Toggle VSync: %s, %s", string(magic_enum::enum_name(Input::Key::X)), vsyncText);
		ImGui::Text("Quit: %s", string(magic_enum::enum_name(Input::Key::Escape)));

		ImGui::End();
	}

	void GUI::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}