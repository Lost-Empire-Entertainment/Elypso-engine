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
#include "configFile.hpp"
#include "stringUtils.hpp"
#include "gui_projectitemslist.hpp"

using std::to_string;
using std::stof;
using std::round;
using glm::value_ptr;
using std::exception;

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
using EngineFile::ConfigFile;
using Utils::String;

namespace Graphics::GUI
{
	void GUISettings::RenderSettings()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

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
				if (ImGui::BeginTabItem("GUI style"))
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
		ImGui::Text("Enable camera movement: Hold right mouse button");
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
		ImGuiStyle& style = ImGui::GetStyle();

		float fontScale = stof(ConfigFile::GetValue("gui_fontScale"));
		ImGui::Text("Font scale");

		if (ImGui::DragFloat("##fontScale", &fontScale, 0.01f, 0.1f, 2.0f))
		{
			if (fontScale < 1.0f) fontScale = 1.0f;
			if (fontScale > 2.0f) fontScale = 2.0f;

			ConfigFile::SetValue("gui_fontScale", to_string(fontScale));
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##fontScale"))
		{
			ConfigFile::SetValue("gui_fontScale", "1.5");
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
	}

	void GUISettings::OtherSettings()
	{
		ImGui::Text("Set game name");
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 250);
		if (gameName == "") gameName = "Game";
		strcpy_s(gameNameChar, bufferSize, gameName.c_str());
		if (ImGui::InputText("##objName", gameNameChar, bufferSize))
		{
			gameName = gameNameChar;
		}
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(Engine::gameExePath.c_str());
		}

		ImGui::SameLine();

		if (ImGui::Button("Apply"))
		{
			bool canApply = true;

			for (char c : gameName)
			{
				if (!String::IsValidSymbolInPath(c))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::INPUT,
						Type::EXCEPTION,
						"Invalid character detected in game name! Please only use english letters, roman numbers and dash, dot or underscore symbol!");

					gameName = "Game";

					canApply = false;

					break;
				}
			}

			if (canApply)
			{
				string finalPath = Engine::gamePath + "/build/Release/" + gameName + ".exe";
				Engine::gameExePath = finalPath;

				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::INFO,
					"Successfully set game name to '" + gameName + "' and game path as '" + Engine::gameExePath + "'!\n");

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
		}

		ImGui::Separator();

		ImGui::Text("Set first scene");
		if (ImGui::Button("Select start scene"))
		{
			GUIProjectItemsList::type = GUIProjectItemsList::Type::Scenes;
			GUIProjectItemsList::selectStartScene = true;
			GUIProjectItemsList::renderProjectItemsList = true;
		}
	}
}