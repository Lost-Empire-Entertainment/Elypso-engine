//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <type_ptr.hpp>
#include <cmath>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "magic_enum.hpp"
#include "glm.hpp"

//engine
#include "gui_settings.hpp"
#include "gui.hpp"
#include "gui_console.hpp"
#include "render.hpp"
#include "input.hpp"
#include "console.hpp"
#include "grid.hpp"
#include "sceneFile.hpp"
#include "core.hpp"
#include "fileexplorer.hpp"

using std::to_string;
using std::stof;
using std::round;
using glm::value_ptr;

using EngineFile::SceneFile;
using Graphics::GUI::GUIConsole;
using Graphics::Render;
using Graphics::Grid;
using Core::Input;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using EngineFile::FileExplorer;

namespace Graphics::GUI
{
	void GUISettings::RenderSettings()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoSavedSettings;

		if (renderSettings
			&& ImGui::Begin("Settings", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderSettings = false;
			}

			if (ImGui::BeginTabBar("Settings"))
			{
				if (ImGui::BeginTabItem("Input"))
				{
					InputSettings();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("GUI"))
				{
					GUIStyleSettings();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Other"))
				{
					OtherSettings();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}
	void GUISettings::InputSettings()
	{
		ImGui::Text("Forwards: W");
		ImGui::Text("Backwards: S");
		ImGui::Text("Left: A");
		ImGui::Text("Right: D");
		ImGui::Text("Up: Space");
		ImGui::Text("Down: Left Control");
		ImGui::Text("Sprint: Left Shift");
		ImGui::Text("Toggle camera: Escape");
		ImGui::Text("Compile game: Ctrl + B");
		ImGui::Text("Run compiled game: Ctrl + R");
		ImGui::Text("Select GameObject: Left Mouse Button");
		ImGui::Text("Delete selected GameObject or node: Delete");
		ImGui::Text("Copy selected object: Ctrl + C");
		ImGui::Text("Paste copied object: Ctrl + V");
		ImGui::Text("Switch to X axis: X");
		ImGui::Text("Switch to Y axis: Y");
		ImGui::Text("Switch to Z axis: Z");
		ImGui::Text("Move selected object:");
		ImGui::Text("Press W and scroll up to increase or down to decrease value");
		ImGui::Text("Rotate selected object:");
		ImGui::Text("Hold E and scroll up to increase or down to decrease value");
		ImGui::Text("Scale selected object:");
		ImGui::Text("Hold R and scroll up to increase or down to decrease value");
	}

	void GUISettings::GUIStyleSettings()
	{
		ImGui::Text("Font scale");
		if (ImGui::DragFloat("##fontScale", &EngineGUI::fontScale, 0.01f, 0.5f, 3.0f))
		{
			ImGuiIO& io = ImGui::GetIO();
			io.FontGlobalScale = EngineGUI::fontScale;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

		ImGui::Text("Alpha");
		if (ImGui::DragFloat("##gui_Alpha", &EngineGUI::gui_Alpha, 0.01f, 0.1f, 1.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Alpha = EngineGUI::gui_Alpha;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Disabled Alpha");
		if (ImGui::DragFloat("##gui_DisabledAlpha", &EngineGUI::gui_DisabledAlpha, 0.01f, 0.1f, 1.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.DisabledAlpha = EngineGUI::gui_DisabledAlpha;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		vec2 gui_WindowPadding = vec2(
			EngineGUI::gui_WindowPadding.x, 
			EngineGUI::gui_WindowPadding.y);
		ImGui::Text("Window padding");
		if (ImGui::DragFloat2("##gui_WindowPadding", value_ptr(gui_WindowPadding), 0.01f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			EngineGUI::gui_WindowPadding = ImVec2(
				gui_WindowPadding.x,
				gui_WindowPadding.y);
			style.WindowPadding = EngineGUI::gui_WindowPadding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Window rounding");
		if (ImGui::DragFloat("##gui_WindowRounding", &EngineGUI::gui_WindowRounding, 0.01f, 0.1f, 10.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = EngineGUI::gui_WindowRounding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Window border size");
		if (ImGui::DragFloat("##gui_WindowBorderSize", &EngineGUI::gui_WindowBorderSize, 0.01f, 0.1f, 5.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowBorderSize = EngineGUI::gui_WindowBorderSize;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
	}

	void GUISettings::OtherSettings()
	{
		ImGui::Text("Print console debug messages");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
		if (ImGui::Checkbox("##consoledebugmsg", &ConsoleManager::sendDebugMessages))
		{
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Set game path");
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 125);
		if (ImGui::Button("Game path", ImVec2(100, 0)))
		{
			Engine::gamePath = FileExplorer::Select(FileExplorer::SearchType::folder);
			Engine::gameExePath = Engine::gamePath + "\\build\\Release\\Game.exe";
			Engine::gameParentPath = Engine::gamePath + "\\build\\Release";
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

			cout << "set game path to " << Engine::gamePath << "\n";
		}
		if (ImGui::IsItemHovered())
		{
			string hint = Engine::gamePath != "" ? Engine::gamePath : "No path set...";
			ImGui::SetTooltip(hint.c_str());
		}
	}
}