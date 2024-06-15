//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <variant>

//external
#include "glm.hpp"
#include "magic_enum.hpp"

//engine
#include "configFile.hpp"
#include "render.hpp"
#include "console.hpp"
#include "stringutils.hpp"
#include "gui.hpp"
#include "gui_console.hpp"
#include "gui_settings.hpp"
#include "gui_inspector.hpp"
#include "gui_assetlist.hpp"
#include "gui_scenemenu.hpp"
#include "core.hpp"
#include "input.hpp"
#include "sceneFile.hpp"

using std::variant;
using glm::vec3;
using std::endl;
using std::cerr;
using std::to_string;
using std::ifstream;
using std::ofstream;
using std::exception;
using std::find_if;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::current_path;
using std::filesystem::create_directory;

using EngineFile::SceneFile;
using Core::Input;
using Core::Engine;
using Graphics::Render;
using Core::ConsoleManager;
using Utils::String;
using Graphics::GUI::EngineGUI;
using Graphics::GUI::GUIConsole;
using Graphics::GUI::GUISettings;
using Graphics::GUI::GUIInspector;
using Graphics::GUI::GUIAssetList;
using Graphics::GUI::GUISceneMenu;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace EngineFile
{
	void ConfigFileManager::UpdateValues(bool reset)
	{
		if (ConfigFileManager::values.size() == 0
			|| reset)
		{
			ConfigFileManager::values.clear();
			ResetConfigValues();
			ResetGUIValues();
			ResetGUIColorValues();

			valuesMap.clear();
			for (const auto& value : values)
			{
				string valueName = value.GetName();
				valuesMap[valueName] = value;

				cout << "added '" << valueName << "' to config values list!\n";
			}
		}
		else
		{
			//updates specific values
			for (const auto& value : ConfigFileManager::values)
			{
				ConfigFileValue val = value;
				if (val.GetName() == "window_vsync")
				{
					glfwSwapInterval(stoi(val.GetValue()));
					break;
				}
				if (val.GetName() == "camera_position")
				{
					vector<string> currentSplitValue = String::Split(val.GetValue(), ',');
					vec3 vec3Value = vec3(stof(currentSplitValue[0]), stof(currentSplitValue[1]), stof(currentSplitValue[2]));
					Render::camera.SetCameraPosition(vec3Value);
				}
				if (val.GetName() == "camera_rotation")
				{
					vector<string> currentSplitValue = String::Split(val.GetValue(), ',');
					vec3 vec3Value = vec3(stof(currentSplitValue[0]), stof(currentSplitValue[1]), stof(currentSplitValue[2]));
					Render::camera.SetCameraPosition(vec3Value);
				}
			}
		}
	}
	void ConfigFileManager::ResetConfigValues()
	{
		ConfigFileValue gui_fontScale(
			"gui_fontScale",
			"1.5",
			"1.0",
			"2.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_fontScale);

		ConfigFileValue graphics_vsync(
			"window_vsync",
			"1",
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(graphics_vsync);

		ConfigFileValue camera_fov(
			"camera_fov",
			"90",
			"70",
			"110",
			ConfigFileValue::Type::type_float);
		AddValue(camera_fov);

		ConfigFileValue camera_nearClip(
			"camera_nearClip",
			"0.001",
			"0.001",
			"10000.0",
			ConfigFileValue::Type::type_float);
		AddValue(camera_nearClip);

		ConfigFileValue camera_farClip(
			"camera_farClip",
			"100.0",
			"0.001",
			"10000.0",
			ConfigFileValue::Type::type_float);
		AddValue(camera_farClip);

		ConfigFileValue camera_position(
			"camera_position",
			"0.0, 1.0, 0.0",
			"-1000000.0, -1000000.0, -1000000.0",
			"1000000.0, 1000000.0, 1000000.0",
			ConfigFileValue::Type::type_vec3);
		AddValue(camera_position);

		ConfigFileValue camera_rotation(
			"camera_rotation",
			"-90.00, 0.00, 0.00",
			"-359.99, -359.99, -359.99",
			"359.99, 359.99, 359.99",
			ConfigFileValue::Type::type_vec3);
		AddValue(camera_rotation);

		ConfigFileValue gui_inspector(
			"gui_inspector",
			"1",
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_inspector);

		ConfigFileValue gui_console(
			"gui_console",
			"1",
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_console);

		ConfigFileValue gui_assetListWindow(
			"gui_assetListWindow",
			"1",
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_assetListWindow);

		ConfigFileValue gui_sceneMenuWindow(
			"gui_sceneMenuWindow",
			"0",
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_sceneMenuWindow);
	}
	void ConfigFileManager::ResetGUIValues()
	{
		ConfigFileValue gui_Alpha(
			"gui_Alpha",
			"1.0",
			"0.000000",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_Alpha);

		ConfigFileValue gui_DisabledAlpha(
			"gui_DisabledAlpha",
			"0.600000",
			"0.000000",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_DisabledAlpha);

		ConfigFileValue gui_WindowPadding(
			"gui_WindowPadding",
			"8.0, 8.0",
			"0.0, 0.0",
			"32.0, 32.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_WindowPadding);

		ConfigFileValue gui_WindowRounding(
			"gui_WindowRounding",
			"4.0",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_WindowRounding);

		ConfigFileValue gui_WindowBorderSize(
			"gui_WindowBorderSize",
			"1.0",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_WindowBorderSize);

		ConfigFileValue gui_WindowMenuButtonPosition(
			"gui_WindowMenuButtonPosition",
			"1",
			imgui_dir,
			ConfigFileValue::Type::type_int);
		AddValue(gui_WindowMenuButtonPosition);

		ConfigFileValue gui_ChildRounding(
			"gui_ChildRounding",
			"0.0",
			"0.0",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ChildRounding);

		ConfigFileValue gui_ChildBorderSize(
			"gui_ChildBorderSize",
			"1.0",
			"0.0",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ChildBorderSize);

		ConfigFileValue gui_PopupRounding(
			"gui_PopupRounding",
			"4.0",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_PopupRounding);

		ConfigFileValue gui_PopupBorderSize(
			"gui_PopupBorderSize",
			"1.0",
			"0.0",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_PopupBorderSize);

		ConfigFileValue gui_FramePadding(
			"gui_FramePadding",
			"4.0, 3.0",
			"0.0, 0.0",
			"10.0, 10.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_FramePadding);

		ConfigFileValue gui_FrameRounding(
			"gui_FrameRounding",
			"2.5",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_FrameRounding);

		ConfigFileValue gui_FrameBorderSize(
			"gui_FrameBorderSize",
			"0.0",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_FrameBorderSize);

		ConfigFileValue gui_ItemSpacing(
			"gui_ItemSpacing",
			"8.0, 4.0",
			"0.0, 0.0",
			"32.0, 32.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_ItemSpacing);

		ConfigFileValue gui_ItemInnerSpacing(
			"gui_ItemInnerSpacing",
			"4.0, 4.0",
			"0.0, 0.0",
			"32.0, 32.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_ItemInnerSpacing);

		ConfigFileValue gui_CellPadding(
			"gui_CellPadding",
			"4.0, 2.0",
			"0.0, 0.0",
			"32.0, 32.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_CellPadding);

		ConfigFileValue gui_IndentSpacing(
			"gui_IndentSpacing",
			"21.0",
			"0.0",
			"50.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_IndentSpacing);

		ConfigFileValue gui_ColumnsMinSpacing(
			"gui_ColumnsMinSpacing",
			"6.0",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ColumnsMinSpacing);

		ConfigFileValue gui_ScrollbarSize(
			"gui_ScrollbarSize",
			"11.0",
			"0.0",
			"25.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ScrollbarSize);

		ConfigFileValue gui_ScrollbarRounding(
			"gui_ScrollbarRounding",
			"2.5",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ScrollbarRounding);

		ConfigFileValue gui_GrabMinSize(
			"gui_GrabMinSize",
			"10.0",
			"0.0",
			"25.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_GrabMinSize);

		ConfigFileValue gui_GrabRounding(
			"gui_GrabRounding",
			"2.0",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_GrabRounding);

		ConfigFileValue gui_TabRounding(
			"gui_TabRounding",
			"3.5",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_TabRounding);

		ConfigFileValue gui_TabBorderSize(
			"gui_TabBorderSize",
			"0.0",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_TabBorderSize);

		ConfigFileValue gui_TabMinWidthForCloseButton(
			"gui_TabMinWidthForCloseButton",
			"0.0",
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_TabMinWidthForCloseButton);

		ConfigFileValue gui_ColorButtonPosition(
			"gui_ColorButtonPosition",
			"1",
			imgui_dir,
			ConfigFileValue::Type::type_int);
		AddValue(gui_ColorButtonPosition);

		ConfigFileValue gui_ButtonTextAlign(
			"gui_ButtonTextAlign",
			"0.5, 0.5",
			"0.0, 0.0",
			"1.0, 1.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_ButtonTextAlign);

		ConfigFileValue gui_SelectableTextAlign(
			"gui_SelectableTextAlign",
			"0.0, 0.0",
			"0.0, 0.0",
			"1.0, 1.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_SelectableTextAlign);
	}
	void ConfigFileManager::ResetGUIColorValues()
	{
		ConfigFileValue gui_Color_Text(
			"gui_Color_Text",
			"1.0, 1.0, 1.0, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Text);

		ConfigFileValue gui_Color_TextDisabled(
			"gui_Color_TextDisabled",
			"0.592156, 0.592156, 0.592156, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TextDisabled);

		ConfigFileValue gui_Color_WindowBg(
			"gui_Color_WindowBg",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_WindowBg);

		ConfigFileValue gui_Color_ChildBg(
			"gui_Color_ChildBg",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ChildBg);

		ConfigFileValue gui_Color_PopupBg(
			"gui_Color_PopupBg",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PopupBg);

		ConfigFileValue gui_Color_Border(
			"gui_Color_Border",
			"0.305882, 0.305882, 0.305882, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Border);

		ConfigFileValue gui_Color_BorderShadow(
			"gui_Color_BorderShadow",
			"0.305882, 0.305882, 0.305882, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_BorderShadow);

		ConfigFileValue gui_Color_FrameBg(
			"gui_Color_FrameBg",
			"0.200000, 0.200000, 0.215686, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_FrameBg);

		ConfigFileValue gui_Color_FrameBgHovered(
			"gui_Color_FrameBgHovered",
			"0.113725, 0.592156, 0.925490, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_FrameBgHovered);

		ConfigFileValue gui_Color_FrameBgActive(
			"gui_Color_FrameBgActive",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_FrameBgActive);

		ConfigFileValue gui_Color_TitleBg(
			"gui_Color_TitleBg",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TitleBg);

		ConfigFileValue gui_Color_TitleBgActive(
			"gui_Color_TitleBgActive",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TitleBgActive);

		ConfigFileValue gui_Color_TitleBgCollapsed(
			"gui_Color_TitleBgCollapsed",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TitleBgCollapsed);

		ConfigFileValue gui_Color_MenuBarBg(
			"gui_Color_MenuBarBg",
			"0.200000, 0.200000, 0.215686, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_MenuBarBg);

		ConfigFileValue gui_Color_ScrollbarBg(
			"gui_Color_ScrollbarBg",
			"0.200000, 0.200000, 0.215686, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ScrollbarBg);

		ConfigFileValue gui_Color_ScrollbarGrab(
			"gui_Color_ScrollbarGrab",
			"0.321568, 0.321568, 0.333333, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ScrollbarGrab);

		ConfigFileValue gui_Color_ScrollbarGrabHovered(
			"gui_Color_ScrollbarGrabHovered",
			"0.352941, 0.352941, 0.372549, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ScrollbarGrabHovered);

		ConfigFileValue gui_Color_ScrollbarGrabActive(
			"gui_Color_ScrollbarGrabActive",
			"0.352941, 0.352941, 0.372549, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ScrollbarGrabActive);

		ConfigFileValue gui_Color_CheckMark(
			"gui_Color_CheckMark",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_CheckMark);

		ConfigFileValue gui_Color_SliderGrab(
			"gui_Color_SliderGrab",
			"0.113725, 0.592156, 0.925490, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_SliderGrab);

		ConfigFileValue gui_Color_SliderGrabActive(
			"gui_Color_SliderGrabActive",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_SliderGrabActive);

		ConfigFileValue gui_Color_Button(
			"gui_Color_Button",
			"0.200000, 0.200000, 0.215686, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Button);

		ConfigFileValue gui_Color_ButtonHovered(
			"gui_Color_ButtonHovered",
			"0.113725, 0.592156, 0.925490, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ButtonHovered);

		ConfigFileValue gui_Color_ButtonActive(
			"gui_Color_ButtonActive",
			"0.113725, 0.592156, 0.925490, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ButtonActive);

		ConfigFileValue gui_Color_Header(
			"gui_Color_Header",
			"0.200000, 0.200000, 0.215686, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Header);

		ConfigFileValue gui_Color_HeaderHovered(
			"gui_Color_HeaderHovered",
			"0.113725, 0.592156, 0.925490, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_HeaderHovered);

		ConfigFileValue gui_Color_HeaderActive(
			"gui_Color_HeaderActive",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_HeaderActive);

		ConfigFileValue gui_Color_Separator(
			"gui_Color_Separator",
			"0.305882, 0.305882, 0.305882, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Separator);

		ConfigFileValue gui_Color_SeparatorHovered(
			"gui_Color_SeparatorHovered",
			"0.305882, 0.305882, 0.305882, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_SeparatorHovered);

		ConfigFileValue gui_Color_SeparatorActive(
			"gui_Color_SeparatorActive",
			"0.305882, 0.305882, 0.305882, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_SeparatorActive);

		ConfigFileValue gui_Color_ResizeGrip(
			"gui_Color_ResizeGrip",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ResizeGrip);

		ConfigFileValue gui_Color_ResizeGripHovered(
			"gui_Color_ResizeGripHovered",
			"0.200000, 0.200000, 0.215686, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ResizeGripHovered);

		ConfigFileValue gui_Color_ResizeGripActive(
			"gui_Color_ResizeGripActive",
			"0.321568, 0.321568, 0.333333, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ResizeGripActive);

		ConfigFileValue gui_Color_Tab(
			"gui_Color_Tab",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Tab);

		ConfigFileValue gui_Color_TabHovered(
			"gui_Color_TabHovered",
			"0.113725, 0.592156, 0.925490, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TabHovered);

		ConfigFileValue gui_Color_TabActive(
			"gui_Color_TabActive",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TabActive);

		ConfigFileValue gui_Color_TabUnfocused(
			"gui_Color_TabUnfocused",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TabUnfocused);

		ConfigFileValue gui_Color_TabUnfocusedActive(
			"gui_Color_TabUnfocusedActive",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TabUnfocusedActive);

		ConfigFileValue gui_Color_PlotLines(
			"gui_Color_PlotLines",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PlotLines);

		ConfigFileValue gui_Color_PlotLinesHovered(
			"gui_Color_PlotLinesHovered",
			"0.113725, 0.592156, 0.925490, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PlotLinesHovered);

		ConfigFileValue gui_Color_PlotHistogram(
			"gui_Color_PlotHistogram",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PlotHistogram);

		ConfigFileValue gui_Color_PlotHistogramHovered(
			"gui_Color_PlotHistogramHovered",
			"0.113725, 0.592156, 0.925490, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PlotHistogramHovered);

		ConfigFileValue gui_Color_TableHeaderBg(
			"gui_Color_TableHeaderBg",
			"0.188235, 0.188235, 0.200000, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableHeaderBg);

		ConfigFileValue gui_Color_TableBorderStrong(
			"gui_Color_TableBorderStrong",
			"0.309803, 0.309803, 0.349019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableBorderStrong);

		ConfigFileValue gui_Color_TableBorderLight(
			"gui_Color_TableBorderLight",
			"0.227450, 0.227450, 0.247058, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableBorderLight);

		ConfigFileValue gui_Color_TableRowBg(
			"gui_Color_TableRowBg",
			"0.0, 0.0, 0.0, 0.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableRowBg);

		ConfigFileValue gui_Color_TableRowBgAlt(
			"gui_Color_TableRowBgAlt",
			"1.0, 1.0, 1.0, 0.059999",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableRowBgAlt);

		ConfigFileValue gui_Color_TextSelectedBg(
			"gui_Color_TextSelectedBg",
			"0.0, 0.466666, 0.784313, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TextSelectedBg);

		ConfigFileValue gui_Color_DragDropTarget(
			"gui_Color_DragDropTarget",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_DragDropTarget);

		ConfigFileValue gui_Color_NavHighlight(
			"gui_Color_NavHighlight",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_NavHighlight);

		ConfigFileValue gui_Color_NavWindowingHighlight(
			"gui_Color_NavWindowingHighlight",
			"1.0, 1.0, 1.0, 0.699999",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_NavWindowingHighlight);

		ConfigFileValue gui_Color_NavWindowingDimBg(
			"gui_Color_NavWindowingDimBg",
			"0.800000, 0.800000, 0.800000, 0.200000",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_NavWindowingDimBg);

		ConfigFileValue gui_Color_ModalWindowDimBg(
			"gui_Color_ModalWindowDimBg",
			"0.145098, 0.145098, 0.149019, 1.0",
			"0.000000, 0.000000, 0.000000, 0.000000",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ModalWindowDimBg);
	}

	void ConfigFileManager::LoadConfigFile()
	{
		//always set config file values to default first 
		//to ensure missing or invalid values are not gonna crash the engine
		UpdateValues(true);

		//create new config file if it does not exist
		if (configFilePath == "")
		{
			configFilePath = Engine::docsPath + "/config.txt";

			if (!exists(configFilePath))
			{
				CreateNewConfigFile();
			}
		}

		ifstream configFile(configFilePath);

		if (!configFile)
		{
			string title = "Failed to open config file";
			string description = "Couldn't open config file at " + configFilePath + "!";
			Engine::CreateErrorPopup(title.c_str(), description.c_str());
		}

		string line;

		while (getline(configFile, line))
		{
			if (line.find(':') != string::npos)
			{
				line.erase(remove(line.begin(), line.end(), ' '), line.end());
				vector<string> lineSplit = String::Split(line, ':');
				vector<string> lineVariables;

				string name = lineSplit[0];
				string variables = lineSplit[1];
				if (variables.find(',') != string::npos)
				{
					variables = lineSplit[1];
					lineVariables = String::Split(variables, ',');
				}
				else lineVariables.push_back(lineSplit[1]);

				bool isInRange = false;
				if (lineVariables.size() == 0
					|| lineVariables.size() > 4)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						name + " has too many or no variables! Resetting to default.");
					isInRange = false;
				}
				else
				{
					for (auto& configFileValue : values)
					{
						string configName = configFileValue.GetName();
						ConfigFileValue::Type configType = configFileValue.GetType();

						if (configName == name
							&& configName != "gui_ColorButtonPosition")
						{
							try
							{
								if (lineVariables.size() == 1)
								{
									if (configType == ConfigFileValue::Type::type_float
										&& String::CanConvertStringToFloat(lineVariables[0])
										&& IsValueInRange(configFileValue, lineVariables))
									{
										configFileValue.SetValue(variables);

										isInRange = true;
										break;
									}
									else if (configType == ConfigFileValue::Type::type_int
										&& String::CanConvertStringToInt(lineVariables[0])
										&& IsValueInRange(configFileValue, lineVariables))
									{
										configFileValue.SetValue(variables);

										isInRange = true;
										break;
									}
								}
								else
								{
									if (lineVariables.size() == 2
										&& configType == ConfigFileValue::Type::type_imvec2
										&& String::CanConvertStringToFloat(lineVariables[0])
										&& String::CanConvertStringToFloat(lineVariables[1])
										&& IsValueInRange(configFileValue, lineVariables))
									{
										configFileValue.SetValue(variables);

										isInRange = true;
										break;
									}
									else if (lineVariables.size() == 2
										&& configType == ConfigFileValue::Type::type_vec2
										&& String::CanConvertStringToFloat(lineVariables[0])
										&& String::CanConvertStringToFloat(lineVariables[1])
										&& IsValueInRange(configFileValue, lineVariables))
									{
										configFileValue.SetValue(variables);

										isInRange = true;
										break;
									}
									else if (lineVariables.size() == 3
										&& configType == ConfigFileValue::Type::type_vec3
										&& String::CanConvertStringToFloat(lineVariables[0])
										&& String::CanConvertStringToFloat(lineVariables[1])
										&& String::CanConvertStringToFloat(lineVariables[2])
										&& IsValueInRange(configFileValue, lineVariables))
									{
										configFileValue.SetValue(variables);

										isInRange = true;
										break;
									}
									else if (lineVariables.size() == 4
										&& configType == ConfigFileValue::Type::type_imvec4
										&& String::CanConvertStringToFloat(lineVariables[0])
										&& String::CanConvertStringToFloat(lineVariables[1])
										&& String::CanConvertStringToFloat(lineVariables[2])
										&& String::CanConvertStringToFloat(lineVariables[3])
										&& IsValueInRange(configFileValue, lineVariables))
									{
										configFileValue.SetValue(variables);

										isInRange = true;
										break;
									}
								}
							}
							catch (const exception& e)
							{
								string title = "Engine crash";
								string message = "Error: '" + string(e.what()) + "' caught at 'configFile.cpp'";
								message += "\n\nConfig type '" + name + "' value '" + variables + "' was not accepted!";

								Engine::CreateErrorPopup(title.c_str(), message.c_str());
							}
						}
					}
				}

				if (isInRange)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set " + name + " to '" + variables + "'.\n");
				}
				else
				{
					for (auto& configFileValue : values)
					{
						if (configFileValue.GetName() == name)
						{
							configFileValue.SetValue(configFileValue.GetDefaultValue());

							ConsoleManager::WriteConsoleMessage(
								Caller::ENGINE,
								Type::EXCEPTION,
								name + " value '" + variables + "' is not valid! Resetting to default '" + configFileValue.GetDefaultValue() +"'\n");

							break;
						}
					}
				}
			}
		}

		configFile.close();

		UpdateValues();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully loaded config file!\n");
	}

	void ConfigFileManager::SaveConfigFile()
	{
		if (exists(configFilePath))
		{
			remove(configFilePath);
		}
		UpdateValues();
		CreateNewConfigFile();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully saved config file!\n");
	}

	void ConfigFileManager::CreateNewConfigFile()
	{
		ofstream configFile(configFilePath);

		if (!configFile)
		{
			string title = "Failed to create config file";
			string description = "Couldn't create config file at " + configFilePath + "!";
			Engine::CreateErrorPopup(title.c_str(), description.c_str());
		}

		for (const auto& variable : values)
		{
			configFile << variable.GetName() << ": " << variable.GetValue() << "\n";
		}

		configFile.close();
	}

	bool ConfigFileManager::IsValueInRange(
		const ConfigFileValue& type,
		const vector<string>& value)
	{
		ConfigFileValue::Type configType = type.GetType();
		string minValue = type.GetMinValue();
		string maxValue = type.GetMaxValue();

		switch (configType)
		{
		case ConfigFileValue::Type::type_float:
		{
			float val = stof(value[0]);
			float minVal = stof(minValue);
			float maxVal = stof(maxValue);
			return val >= minVal && val <= maxVal;
		}
		case ConfigFileValue::Type::type_int:
		{
			if (type.GetName() == "gui_WindowMenuButtonPosition"
				|| type.GetName() == "gui_TabMinWidthForCloseButton")
			{
				int val = stoi(value[0]);
				return val >= 0 && val <= 2;
			}
			else
			{
				int val = stoi(value[0]);
				int minVal = stoi(minValue);
				int maxVal = stoi(maxValue);
				return val >= minVal && val <= maxVal;
			}
		}
		case ConfigFileValue::Type::type_imvec2:
		{
			float valX = stof(value[0]);
			float valY = stof(value[1]);

			vector<string> minVec = String::Split(minValue, ',');
			float minX = stof(minVec[0]);
			float minY = stof(minVec[1]);

			vector<string> maxVec = String::Split(maxValue, ',');
			float maxX = stof(maxVec[0]);
			float maxY = stof(maxVec[1]);

			return
				valX >= minX
				&& valX <= maxX
				&& valY >= minY
				&& valY <= maxY;
		}
		case ConfigFileValue::Type::type_vec2:
		{
			float valX = stof(value[0]);
			float valY = stof(value[1]);

			vector<string> minVec = String::Split(minValue, ',');
			float minX = stof(minVec[0]);
			float minY = stof(minVec[1]);

			vector<string> maxVec = String::Split(maxValue, ',');
			float maxX = stof(maxVec[0]);
			float maxY = stof(maxVec[1]);

			return
				valX >= minX
				&& valX <= maxX
				&& valY >= minY
				&& valY <= maxY;
		}
		case ConfigFileValue::Type::type_vec3:
		{
			float valX = stof(value[0]);
			float valY = stof(value[1]);
			float valZ = stof(value[2]);

			vector<string> minVec = String::Split(minValue, ',');
			float minX = stof(minVec[0]);
			float minY = stof(minVec[1]);
			float minZ = stof(minVec[2]);

			vector<string> maxVec = String::Split(maxValue, ',');
			float maxX = stof(maxVec[0]);
			float maxY = stof(maxVec[1]);
			float maxZ = stof(maxVec[2]);

			return
				valX >= minX
				&& valX <= maxX
				&& valY >= minY
				&& valY <= maxY
				&& valZ >= minZ
				&& valZ <= maxZ;
		}
		case ConfigFileValue::Type::type_imvec4:
		{
			float valX = stof(value[0]);
			float valY = stof(value[1]);
			float valZ = stof(value[2]);
			float valW = stof(value[3]);

			vector<string> minVec = String::Split(minValue, ',');
			float minX = stof(minVec[0]);
			float minY = stof(minVec[1]);
			float minZ = stof(minVec[2]);
			float minW = stof(minVec[3]);

			vector<string> maxVec = String::Split(maxValue, ',');
			float maxX = stof(maxVec[0]);
			float maxY = stof(maxVec[1]);
			float maxZ = stof(maxVec[2]);
			float maxW = stof(maxVec[3]);

			return
				valX >= minX
				&& valX <= maxX
				&& valY >= minY
				&& valY <= maxY
				&& valZ >= minZ
				&& valZ <= maxZ
				&& valW >= minW
				&& valW <= maxW;
		}
		}

		return false;
	}
}