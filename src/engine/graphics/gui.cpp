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

//engine
#include "core.h"
#include "gui.h"
#include "render.h"

#include <string>

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

	void GUI::Render()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		GUI::RenderContent();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GUI::RenderContent() 
	{
		//docked and not movable
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings;

		//window initial size
		ImVec2 initialSize(300, 300);

		//start a new window with specified flags and size
		ImGui::SetNextWindowSize(initialSize, ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGui::Begin("Info", nullptr, windowFlags);

		//set font size
		ImGui::SetWindowFontScale(1.5);

		ImGui::Text("Version: %s", Engine::version.c_str());
		ImGui::Text("bruh bro");
		ImGui::Text("bruh bro");

		ImGui::End();
	}

	void GUI::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}