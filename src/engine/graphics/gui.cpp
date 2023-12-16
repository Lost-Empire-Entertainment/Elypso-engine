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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "magic_enum.hpp"

//engine
#include "console.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "input.hpp"
#include "render.hpp"
#include "stringUtils.hpp"
#include "timeManager.hpp"
#include "searchUtils.hpp"

#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::to_string;

using Core::Engine;
using Core::Input;
using Core::ConsoleManager;
using Core::TimeManager;
using Utils::Search;
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
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = Search::FindDocumentsFolder() + "/imgui.ini";
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ConsoleManager::WriteConsoleMessage(
			Caller::IMGUI,
			Type::DEBUG,
			"Imgui.ini path: " + tempString + ".\n");

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		//clear default font
		io.Fonts->Clear();

		string filesPath = Search::SearchByParent("files");
		io.Fonts->AddFontFromFileTTF((filesPath + "/fonts/coda/Coda-Regular.ttf").c_str(), 16.0f);

		CustomizeImGuiStyle();
	}

	void GUI::CustomizeImGuiStyle()
	{
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		style.TabRounding = 6;
		style.FramePadding = ImVec2(6, 2);
		style.ItemSpacing = ImVec2(0, 5);
		io.FontGlobalScale = fontScale;
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

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		GUI::RenderDebugMenu();
		GUI::RenderSlider();
		GUI::RenderConsole();

		if (Input::inputSettings.printIMGUIToConsole)
		{
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
		ImVec2 initialPos(5, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 minSize(350, 600);
		ImGui::SetNextWindowSizeConstraints(initialSize, ImVec2(INT_MAX, INT_MAX));
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGui::Begin("Debug menu");

		GUI::RDM_Info();
		GUI::RDM_GeneralKeys();
		GUI::RDM_DebugButtons();

		ImGui::End();
	}

	// render sliders menu at the top right corner like RenderDebugMenu()
	void GUI::RenderSlider()
	{
		ImVec2 initialPos(925, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 minSize(350, 600);
		ImGui::SetNextWindowSizeConstraints(initialSize, ImVec2(INT_MAX, INT_MAX));
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGui::Begin("Sliders menu");

		RS_CameraClipRange();
		RS_MoveSpeedMultiplier();
		RS_FOV();

		ImGui::End();
	}

	void GUI::RenderConsole()
	{
		ImVec2 initialPos(357, 500);
		ImVec2 initialSize(530, 200);
		ImVec2 minSize(530, 200);
		ImGui::SetNextWindowSizeConstraints(initialSize, ImVec2(INT_MAX, INT_MAX));
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

		ImGui::Begin("Console");

		//text area with scrollable region
		ImVec2 scrollingRegionSize(
			ImGui::GetContentRegionAvail().x,
			ImGui::GetContentRegionAvail().y - 25);
		ImGui::BeginChild("ScrollingRegion", scrollingRegionSize, true);

		float wrapWidth = ImGui::GetContentRegionAvail().x - 10;
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);

		//display the content of the text buffer
		ImGui::TextWrapped("%s", textBuffer.begin(), textBuffer.end());

		ImGui::PopTextWrapPos();

		//scrolls to the bottom if scrolling is allowed
		//and if user is close to the newest console message
		if (allowScrollToBottom)
		{
			bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f;
			if (isNearBottom) ImGui::SetScrollHereY(1.0f);
		}

		AddTextToConsole();

		ImGui::EndChild();

		//text filter input box
		float textAreaHeight = ImGui::GetContentRegionAvail().y - 25.0f;
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + textAreaHeight));

		// Draw the text filter input box
		ImGui::PushItemWidth(scrollingRegionSize.x);
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackCharFilter;
		ImGui::InputText("##filter", textFilter.InputBuf, IM_ARRAYSIZE(textFilter.InputBuf), flags,
			[](ImGuiInputTextCallbackData* data)
			{
				char c = data->EventChar;
				if (c == '\n' || c == '\r')
				{
					return 1;
				}
				return 0;
			});

		//retrieve the text entered by the user
		const char* filterText = textFilter.InputBuf;

		//check if Enter/Return was pressed, if so, reset the processed flag and clear the input field
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) 
			|| ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_KeyPadEnter)))
		{
			//parse the filter text
			if (!filterTextProcessed
				&& filterText
				&& filterText[0] != '\0')
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::INFO,
					string(filterText) + "\n");
			}

			memset(textFilter.InputBuf, 0, sizeof(textFilter.InputBuf));
		}

		ImGui::PopItemWidth();

		ImGui::End();
	}
	void GUI::AddTextToConsole()
	{
		if (writeToConsole)
		{
			textBuffer.appendf("%s\n", addedText.c_str());

			//second scroll update that scrolls to the bottom if scrolling is allowed
			//and when any new text is added. top one fully scrolls to the bottom,
			//this one is used because there is no better way to force scroll to the bottom
			//when enter is pressed and only using this doesnt fully scroll to the bottom
			//so this works together with the top scroll update to always scroll to the bottom
			if (allowScrollToBottom) ImGui::SetScrollHereY(1.0f);

			writeToConsole = false;
		}
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
		if (ImGui::Button(allowScrollToBottom ?
			"Disable scroll to bottom" :
			"Enable scroll to bottom"))
		{
			allowScrollToBottom = !allowScrollToBottom;
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