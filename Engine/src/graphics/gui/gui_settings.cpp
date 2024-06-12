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
using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;
using Utils::String;

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
				if (ImGui::BeginTabItem("GUI style"))
				{
					GUIStyleSettings();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("GUI color"))
				{
					GUIColorSettings();
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

	void GUISettings::HandleConfigValue(
		const string& name, 
		const ConfigFileValue::Type& type,
		ImGuiCol col)
	{
		ImGui::Text(name.c_str());
		ConfigFileValue config = ConfigFileManager::valuesMap[name];

		switch (type)
		{
		case ConfigFileValue::Type::type_float:
		{
			float max = stof(config.GetMaxValue());
			float min = stof(config.GetMinValue());
			float increment = (max - min) / 100.0f;
			float value = stof(config.GetValue());
			if (ImGui::DragFloat(("##" + name).c_str(), &value, increment, min, max))
			{
				ImGuiIO& io = ImGui::GetIO();
				AssignGUIValue(name, type);
				ConfigFileManager::valuesMap[name].SetValue(to_string(value));

				cout << "1.1- set '" + config.GetName() + "' to '" 
					+ ConfigFileManager::valuesMap[name].GetValue() + "'\n";

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			break;
		}
		case ConfigFileValue::Type::type_int:
		{
			break;
		}
		case ConfigFileValue::Type::type_imvec2:
		{
			string stringValue = config.GetValue();
			vector<string> split = String::Split(stringValue, ',');
			vec2 value = vec2(stof(split[0]), stof(split[1]));
			if (ImGui::DragFloat2(("##" + name).c_str(), value_ptr(value), 0.01f))
			{
				ImGuiStyle& style = ImGui::GetStyle();
				AssignGUIValue(name, type);
				ConfigFileManager::valuesMap[name].SetValue(
					to_string(value.x) + ", "
					+ to_string(value.y));

				cout << "1.2- set '" + config.GetName() + "' to '"
					+ ConfigFileManager::valuesMap[name].GetValue() + "'\n";

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			break;
		}
		case ConfigFileValue::Type::type_vec2:
		{
			string stringValue = config.GetValue();
			vector<string> split = String::Split(stringValue, ',');
			vec2 value = vec2(stof(split[0]), stof(split[1]));
			if (ImGui::DragFloat2(("##" + name).c_str(), value_ptr(value), 0.01f))
			{
				ImGuiStyle& style = ImGui::GetStyle();
				AssignGUIValue(name, type);
				ConfigFileManager::valuesMap[name].SetValue(
					to_string(value.x) + ", "
					+ to_string(value.y));

				cout << "1.3- set '" + config.GetName() + "' to '"
					+ ConfigFileManager::valuesMap[name].GetValue() + "'\n";

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			break;
		}
		case ConfigFileValue::Type::type_vec3:
		{
			break;
		}
		case ConfigFileValue::Type::type_imvec4:
		{
			string stringValue = config.GetValue();
			vector<string> split = String::Split(stringValue, ',');
			vec4 value = vec4(stof(split[0]), stof(split[1]), stof(split[2]), stof(split[3]));

			if (ImGui::ColorEdit4(("##" + name).c_str(), (float*)&value))
			{
				ImGuiStyle& style = ImGui::GetStyle();
				AssignGUIValue(name, type);
				ConfigFileManager::valuesMap[name].SetValue(
					to_string(value.x) + ", "
					+ to_string(value.y) + ", "
					+ to_string(value.z) + ", "
					+ to_string(value.w));

				cout << "1.4- set '" + config.GetName() + "' to '"
					+ ConfigFileManager::valuesMap[name].GetValue() + "'\n";

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			break;
		}
		}
	}
	void GUISettings::AssignGUIValue(
		const string& name, 
		const ConfigFileValue::Type& type,
		ImGuiCol col)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		ConfigFileValue config = ConfigFileManager::valuesMap[name];

		if (type == ConfigFileValue::Type::type_float)
		{
			float value = stof(config.GetValue());

			if (name == "gui_fontScale") io.FontGlobalScale = value;
			if (name == "gui_Alpha") style.Alpha = value;
			if (name == "gui_DisabledAlpha") style.DisabledAlpha = value;
			if (name == "gui_WindowRounding") style.WindowRounding = value;
			if (name == "gui_WindowBorderSize") style.WindowBorderSize = value;
			if (name == "gui_ChildRounding") style.ChildRounding = value;
			if (name == "gui_ChildBorderSize") style.ChildBorderSize = value;
			if (name == "gui_PopupRounding") style.PopupRounding = value;
			if (name == "gui_PopupBorderSize") style.PopupBorderSize = value;
			if (name == "gui_FrameRounding") style.FrameRounding = value;
			if (name == "gui_FrameBorderSize") style.FrameBorderSize = value;
			if (name == "gui_IndentSpacing") style.IndentSpacing = value;
			if (name == "gui_ColumnsMinSpacing") style.ColumnsMinSpacing = value;
			if (name == "gui_ScrollbarSize") style.ScrollbarSize = value;
			if (name == "gui_ScrollbarRounding") style.ScrollbarRounding = value;
			if (name == "gui_GrabMinSize") style.GrabMinSize = value;
			if (name == "gui_GrabRounding") style.GrabRounding = value;
			if (name == "gui_TabRounding") style.TabRounding = value;
			if (name == "gui_TabBorderSize") style.TabBorderSize = value;
		}

		if (type == ConfigFileValue::Type::type_int)
		{
			int value = stoi(config.GetValue());
		}

		if (type == ConfigFileValue::Type::type_imvec2)
		{
			string stringValue = config.GetValue();
			vector<string> split = String::Split(stringValue, ',');
			ImVec2 value = ImVec2(stof(split[0]), stof(split[1]));

			if (name == "gui_WindowPadding") style.WindowPadding = value;
			if (name == "gui_FramePadding") style.FramePadding = value;
			if (name == "gui_ItemSpacing") style.ItemSpacing = value;
			if (name == "gui_ItemInnerSpacing") style.ItemInnerSpacing = value;
			if (name == "gui_CellPadding") style.CellPadding = value;
			if (name == "gui_ButtonTextAlign") style.ButtonTextAlign = value;
			if (name == "gui_SelectableTextAlign") style.SelectableTextAlign = value;
		}

		if (type == ConfigFileValue::Type::type_imvec4)
		{
			string stringValue = config.GetValue();
			vector<string> split = String::Split(stringValue, ',');
			ImVec4 value = ImVec4(stof(split[0]), stof(split[1]), stof(split[2]), stof(split[3]));

			style.Colors[col] = value;
		}

		cout << "2.1- set '" + config.GetName() + "' to '"
			+ ConfigFileManager::valuesMap[name].GetValue() + "'\n";
	}
	void GUISettings::GUIStyleSettings()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		HandleConfigValue("gui_fontScale", ConfigFileValue::Type::type_float);

		ImGui::Separator();

		HandleConfigValue("gui_Alpha", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_DisabledAlpha", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_WindowPadding", ConfigFileValue::Type::type_imvec2); 
		HandleConfigValue("gui_WindowRounding", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_WindowBorderSize", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_WindowMenuButtonPosition", ConfigFileValue::Type::type_int);
		/*
		const char* choice_windowMenuButtonPosition[] = 
		{ 
			"Left", 
			"Right", 
			"None"
		};
		static int index_windowMenuButtonPosition = 0;
		ImGui::Text("Window menu button position");
		if (ImGui::Combo(
			"##gui_WindowMenuButtonPosition", 
			&index_windowMenuButtonPosition,
			choice_windowMenuButtonPosition,
			IM_ARRAYSIZE(choice_windowMenuButtonPosition)))
		{
			ImGuiStyle& style = ImGui::GetStyle();

			string choice = choice_windowMenuButtonPosition[index_windowMenuButtonPosition];
			auto imgui_dir = magic_enum::enum_cast<ImGuiDir_>("ImGuiDir_" + choice);
			ImGuiDir_ chosenDir = imgui_dir.value();
			EngineGUI::gui_WindowMenuButtonPosition = chosenDir;
			style.WindowMenuButtonPosition = chosenDir;
		}
		*/

		ImGui::Separator();

		HandleConfigValue("gui_ChildRounding", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_ChildBorderSize", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_PopupRounding", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_PopupBorderSize", ConfigFileValue::Type::type_float);

		ImGui::Separator();

		HandleConfigValue("gui_FramePadding", ConfigFileValue::Type::type_imvec2);
		HandleConfigValue("gui_FrameRounding", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_FrameBorderSize", ConfigFileValue::Type::type_float);

		ImGui::Separator();

		HandleConfigValue("gui_ItemSpacing", ConfigFileValue::Type::type_imvec2);
		HandleConfigValue("gui_ItemInnerSpacing", ConfigFileValue::Type::type_imvec2);

		ImGui::Separator();

		HandleConfigValue("gui_CellPadding", ConfigFileValue::Type::type_imvec2);
		HandleConfigValue("gui_IndentSpacing", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_ColumnsMinSpacing", ConfigFileValue::Type::type_float);

		ImGui::Separator();

		HandleConfigValue("gui_ScrollbarSize", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_ScrollbarRounding", ConfigFileValue::Type::type_float);

		ImGui::Separator();

		HandleConfigValue("gui_GrabMinSize", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_GrabRounding", ConfigFileValue::Type::type_float);

		ImGui::Separator();

		HandleConfigValue("gui_TabRounding", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_TabBorderSize", ConfigFileValue::Type::type_float);
		HandleConfigValue("gui_TabMinWidthForCloseButton", ConfigFileValue::Type::type_float);

		ImGui::Separator();

		HandleConfigValue("gui_ColorButtonPosition", ConfigFileValue::Type::type_int);
		/*
		const char* choice_colorButtonPosition[] =
		{
			"Left",
			"Right",
			"None"
		};
		static int index_colorButtonPosition = 0;
		ImGui::Text("Window menu button position");
		if (ImGui::Combo(
			"##gui_ColorButtonPosition",
			&index_colorButtonPosition,
			choice_colorButtonPosition,
			IM_ARRAYSIZE(choice_colorButtonPosition)))
		{
			ImGuiStyle& style = ImGui::GetStyle();

			string choice = choice_colorButtonPosition[index_colorButtonPosition];
			auto imgui_dir = magic_enum::enum_cast<ImGuiDir_>("ImGuiDir_" + choice);
			ImGuiDir_ chosenDir = imgui_dir.value();
			EngineGUI::gui_ColorButtonPosition = chosenDir;
			style.ColorButtonPosition = chosenDir;
		}
		*/
		HandleConfigValue("gui_ButtonTextAlign", ConfigFileValue::Type::type_imvec2);
		HandleConfigValue("gui_SelectableTextAlign", ConfigFileValue::Type::type_imvec2);
	}

	void GUISettings::GUIColorSettings()
	{
		HandleConfigValue("gui_Color_Text", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TextDisabled", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_WindowBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ChildBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_PopupBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_Border", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_BorderShadow", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_FrameBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_FrameBgHovered", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_FrameBgActive", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TitleBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TitleBgActive", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TitleBgCollapsed", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_MenuBarBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ScrollbarBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ScrollbarGrab", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ScrollbarGrabHovered", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ScrollbarGrabActive", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_CheckMark", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_SliderGrab", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_SliderGrabActive", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_Button", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ButtonHovered", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ButtonActive", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_Header", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_HeaderHovered", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_HeaderActive", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_Separator", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_SeparatorHovered", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_SeparatorActive", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ResizeGrip", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ResizeGripHovered", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ResizeGripActive", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_Tab", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TabHovered", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TabActive", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TabUnfocused", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TabUnfocusedActive", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_PlotLines", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_PlotLinesHovered", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_PlotHistogram", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_PlotHistogramHovered", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_TableHeaderBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TableBorderStrong", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TableBorderLight", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TableRowBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_TableRowBgAlt", ConfigFileValue::Type::type_imvec4);

		ImGui::Separator();

		HandleConfigValue("gui_Color_TextSelectedBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_DragDropTarget", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_NavHighlight", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_NavWindowingHighlight", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_NavWindowingDimBg", ConfigFileValue::Type::type_imvec4);
		HandleConfigValue("gui_Color_ModalWindowDimBg", ConfigFileValue::Type::type_imvec4);
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