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

//engine
#include "configFile.hpp"
#include "render.hpp"
#include "console.hpp"
#include "stringutils.hpp"
#include "gui.hpp"
#include "gui_console.hpp"
#include "gui_settings.hpp"
#include "gui_inspector.hpp"
#include "gui_nodeblock.hpp"
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
using Graphics::GUI::GUINodeBlock;
using Graphics::GUI::GUIAssetList;
using Graphics::GUI::GUISceneMenu;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace EngineFile
{
	ConfigFileManager configFileManager;
	static vector<string> imgui_dir =
	{
		"ImGuiDir_Up",
		"ImGuiDir_Down",
		"ImGuiDir_Left",
		"ImGuiDir_Right",
		"ImGuiDir_None"
	};

	void ConfigFileManager::SetDefaultConfigValues()
	{
		Render::useMonitorRefreshRate = true;
		Input::moveSpeedMultiplier = 1.0f;
		Input::nearClip = 0.001f;
		Input::farClip = 100.0f;
		vec3 camPos = vec3(0.0f, 1.0f, 0.0f);
		Render::camera.SetCameraPosition(camPos);

		EngineGUI::fontScale = 1.5f;

		//
		// GUI STYLE START
		//

		EngineGUI::gui_Alpha = 1.0f;
		EngineGUI::gui_DisabledAlpha = 0.6000000238418579f;
		EngineGUI::gui_WindowPadding = ImVec2(8.0f, 8.0f);
		EngineGUI::gui_WindowRounding = 4.0f;
		EngineGUI::gui_WindowBorderSize = 1.0f;
		EngineGUI::gui_WindowMenuButtonPosition = ImGuiDir_Left;
		EngineGUI::gui_ChildRounding = 0.0f;
		EngineGUI::gui_ChildBorderSize = 1.0f;
		EngineGUI::gui_PopupRounding = 4.0f;
		EngineGUI::gui_PopupBorderSize = 1.0f;
		EngineGUI::gui_FramePadding = ImVec2(4.0f, 3.0f);
		EngineGUI::gui_FrameRounding = 2.5f;
		EngineGUI::gui_FrameBorderSize = 0.0f;
		EngineGUI::gui_ItemSpacing = ImVec2(8.0f, 4.0f);
		EngineGUI::gui_ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		EngineGUI::gui_CellPadding = ImVec2(4.0f, 2.0f);
		EngineGUI::gui_IndentSpacing = 21.0f;
		EngineGUI::gui_ColumnsMinSpacing = 6.0f;
		EngineGUI::gui_ScrollbarSize = 11.0f;
		EngineGUI::gui_ScrollbarRounding = 2.5f;
		EngineGUI::gui_GrabMinSize = 10.0f;
		EngineGUI::gui_GrabRounding = 2.0f;
		EngineGUI::gui_TabRounding = 3.5f;
		EngineGUI::gui_TabBorderSize = 0.0f;
		EngineGUI::gui_TabMinWidthForCloseButton = 0.0f;
		EngineGUI::gui_ColorButtonPosition = ImGuiDir_Right;
		EngineGUI::gui_ButtonTextAlign = ImVec2(0.5f, 0.5f);
		EngineGUI::gui_SelectableTextAlign = ImVec2(0.0f, 0.0f);

		EngineGUI::gui_Color_Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		EngineGUI::gui_Color_TextDisabled = ImVec4(0.5921568870544434f, 0.5921568870544434f, 0.5921568870544434f, 1.0f);
		EngineGUI::gui_Color_WindowBg = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_ChildBg = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_PopupBg = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_Border = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		EngineGUI::gui_Color_BorderShadow = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		EngineGUI::gui_Color_FrameBg = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		EngineGUI::gui_Color_FrameBgHovered = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		EngineGUI::gui_Color_FrameBgActive = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_TitleBg = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_TitleBgActive = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_TitleBgCollapsed = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_MenuBarBg = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		EngineGUI::gui_Color_ScrollbarBg = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		EngineGUI::gui_Color_ScrollbarGrab = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
		EngineGUI::gui_Color_ScrollbarGrabHovered = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
		EngineGUI::gui_Color_ScrollbarGrabActive = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
		EngineGUI::gui_Color_CheckMark = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_SliderGrab = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		EngineGUI::gui_Color_SliderGrabActive = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_Button = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		EngineGUI::gui_Color_ButtonHovered = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		EngineGUI::gui_Color_ButtonActive = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		EngineGUI::gui_Color_Header = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		EngineGUI::gui_Color_HeaderHovered = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		EngineGUI::gui_Color_HeaderActive = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_Separator = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		EngineGUI::gui_Color_SeparatorHovered = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		EngineGUI::gui_Color_SeparatorActive = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		EngineGUI::gui_Color_ResizeGrip = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_ResizeGripHovered = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		EngineGUI::gui_Color_ResizeGripActive = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
		EngineGUI::gui_Color_Tab = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_TabHovered = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		EngineGUI::gui_Color_TabActive = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_TabUnfocused = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_TabUnfocusedActive = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_PlotLines = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_PlotLinesHovered = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		EngineGUI::gui_Color_PlotHistogram = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_PlotHistogramHovered = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		EngineGUI::gui_Color_TableHeaderBg = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		EngineGUI::gui_Color_TableBorderStrong = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		EngineGUI::gui_Color_TableBorderLight = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		EngineGUI::gui_Color_TableRowBg = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		EngineGUI::gui_Color_TableRowBgAlt = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		EngineGUI::gui_Color_TextSelectedBg = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		EngineGUI::gui_Color_DragDropTarget = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_NavHighlight = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		EngineGUI::gui_Color_NavWindowingHighlight = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
		EngineGUI::gui_Color_NavWindowingDimBg = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		EngineGUI::gui_Color_ModalWindowDimBg = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);

		//
		// GUI STYLE END
		//

		configFileManager.UpdateValues();
	}

	void ConfigFileManager::UpdateValues()
	{
		values.clear();

		ConfigFileValue gui_fontScale(
			"gui_fontScale",
			to_string(EngineGUI::fontScale),
			"1.0",
			"2.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_fontScale);

		ConfigFileValue graphics_vsync(
			"window_vsync",
			to_string(Render::useMonitorRefreshRate),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(graphics_vsync);

		ConfigFileValue camera_fov(
			"camera_fov",
			to_string(Input::fov),
			"70",
			"110",
			ConfigFileValue::Type::type_float);
		AddValue(camera_fov);

		ConfigFileValue camera_nearClip(
			"camera_nearClip",
			to_string(Input::nearClip),
			"0.001",
			"10000.0",
			ConfigFileValue::Type::type_float);
		AddValue(camera_nearClip);

		ConfigFileValue camera_farClip(
			"camera_farClip",
			to_string(Input::farClip),
			"0.001",
			"10000.0",
			ConfigFileValue::Type::type_float);
		AddValue(camera_farClip);

		string camera_position_value =
			to_string(Render::camera.GetCameraPosition().x) + ", " +
			to_string(Render::camera.GetCameraPosition().y) + ", " +
			to_string(Render::camera.GetCameraPosition().z);
		ConfigFileValue camera_position(
			"camera_position",
			camera_position_value,
			"-1000000.0, -1000000.0, -1000000.0",
			"1000000.0, 1000000.0, 1000000.0",
			ConfigFileValue::Type::type_vec3);
		AddValue(camera_position);

		string camera_rotation_value =
			to_string(Render::camera.GetCameraRotation().x) + ", " +
			to_string(Render::camera.GetCameraRotation().y) + ", " +
			to_string(Render::camera.GetCameraRotation().z);
		ConfigFileValue camera_rotation(
			"camera_rotation",
			camera_rotation_value,
			"-359.99, -359.99, -359.99",
			"359.99, 359.99, 359.99",
			ConfigFileValue::Type::type_vec3);
		AddValue(camera_rotation);

		ConfigFileValue gui_inspector(
			"gui_inspector",
			to_string(GUIInspector::renderInspector),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_inspector);

		ConfigFileValue gui_console(
			"gui_console",
			to_string(GUIConsole::renderConsole),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_console);

		ConfigFileValue gui_nodeBlockWindow(
			"gui_nodeBlockWindow",
			to_string(GUINodeBlock::renderNodeBlock),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_nodeBlockWindow);

		ConfigFileValue gui_assetListWindow(
			"gui_assetListWindow",
			to_string(GUIAssetList::renderAssetList),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_assetListWindow);

		ConfigFileValue gui_sceneMenuWindow(
			"gui_sceneMenuWindow",
			to_string(GUISceneMenu::renderSceneMenu),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_sceneMenuWindow);

		//
		// GUI STYLE START
		//

		ConfigFileValue gui_Alpha(
			"gui_Alpha",
			to_string(EngineGUI::gui_Alpha),
			"0.0000000000000001",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_Alpha);

		ConfigFileValue gui_DisabledAlpha(
			"gui_DisabledAlpha",
			to_string(EngineGUI::gui_DisabledAlpha),
			"0.0000000000000001",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_DisabledAlpha);

		string gui_WindowPadding_value =
			to_string(EngineGUI::gui_WindowPadding.x) + ", " +
			to_string(EngineGUI::gui_WindowPadding.y);
		ConfigFileValue gui_WindowPadding(
			"gui_WindowPadding",
			gui_WindowPadding_value,
			"0.0, 0.0",
			"32.0, 32.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_WindowPadding);

		ConfigFileValue gui_WindowRounding(
			"gui_WindowRounding",
			to_string(EngineGUI::gui_WindowRounding),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_WindowRounding);

		ConfigFileValue gui_WindowBorderSize(
			"gui_WindowBorderSize",
			to_string(EngineGUI::gui_WindowBorderSize),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_WindowBorderSize);

		ConfigFileValue gui_WindowMenuButtonPosition(
			"gui_WindowMenuButtonPosition",
			to_string(EngineGUI::gui_WindowMenuButtonPosition),
			imgui_dir,
			ConfigFileValue::Type::type_string);
		AddValue(gui_WindowMenuButtonPosition);

		ConfigFileValue gui_ChildRounding(
			"gui_ChildRounding",
			to_string(EngineGUI::gui_ChildRounding),
			"0.0",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ChildRounding);

		ConfigFileValue gui_ChildBorderSize(
			"gui_ChildBorderSize",
			to_string(EngineGUI::gui_ChildBorderSize),
			"0.0",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ChildBorderSize);

		ConfigFileValue gui_PopupRounding(
			"gui_PopupRounding",
			to_string(EngineGUI::gui_PopupRounding),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_PopupRounding);

		ConfigFileValue gui_PopupBorderSize(
			"gui_PopupBorderSize",
			to_string(EngineGUI::gui_PopupBorderSize),
			"0.0",
			"1.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_PopupBorderSize);

		string gui_FramePadding_value =
			to_string(EngineGUI::gui_FramePadding.x) + ", " +
			to_string(EngineGUI::gui_FramePadding.y);
		ConfigFileValue gui_FramePadding(
			"gui_FramePadding",
			gui_FramePadding_value,
			"0.0, 0.0",
			"10.0, 10.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_FramePadding);

		ConfigFileValue gui_FrameRounding(
			"gui_FrameRounding",
			to_string(EngineGUI::gui_FrameRounding),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_FrameRounding);

		ConfigFileValue gui_FrameBorderSize(
			"gui_FrameBorderSize",
			to_string(EngineGUI::gui_FrameBorderSize),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_FrameBorderSize);

		string gui_ItemSpacing_value =
			to_string(EngineGUI::gui_ItemSpacing.x) + ", " +
			to_string(EngineGUI::gui_ItemSpacing.y);
		ConfigFileValue gui_ItemSpacing(
			"gui_ItemSpacing",
			gui_ItemSpacing_value,
			"0.0, 0.0",
			"32.0, 32.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_ItemSpacing);

		string gui_ItemInnerSpacing_value =
			to_string(EngineGUI::gui_ItemInnerSpacing.x) + ", " +
			to_string(EngineGUI::gui_ItemInnerSpacing.y);
		ConfigFileValue gui_ItemInnerSpacing(
			"gui_ItemInnerSpacing",
			gui_ItemInnerSpacing_value,
			"0.0, 0.0",
			"32.0, 32.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_ItemInnerSpacing);

		string gui_CellPadding_value =
			to_string(EngineGUI::gui_CellPadding.x) + ", " +
			to_string(EngineGUI::gui_CellPadding.y);
		ConfigFileValue gui_CellPadding(
			"gui_CellPadding",
			gui_CellPadding_value,
			"0.0, 0.0",
			"32.0, 32.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_CellPadding);

		ConfigFileValue gui_IndentSpacing(
			"gui_IndentSpacing",
			to_string(EngineGUI::gui_IndentSpacing),
			"0.0",
			"50.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_IndentSpacing);

		ConfigFileValue gui_ColumnsMinSpacing(
			"gui_ColumnsMinSpacing",
			to_string(EngineGUI::gui_ColumnsMinSpacing),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ColumnsMinSpacing);

		ConfigFileValue gui_ScrollbarSize(
			"gui_ScrollbarSize",
			to_string(EngineGUI::gui_ScrollbarSize),
			"0.0",
			"25.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ScrollbarSize);

		ConfigFileValue gui_ScrollbarRounding(
			"gui_ScrollbarRounding",
			to_string(EngineGUI::gui_ScrollbarRounding),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_ScrollbarRounding);

		ConfigFileValue gui_GrabMinSize(
			"gui_GrabMinSize",
			to_string(EngineGUI::gui_GrabMinSize),
			"0.0",
			"25.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_GrabMinSize);

		ConfigFileValue gui_GrabRounding(
			"gui_GrabRounding",
			to_string(EngineGUI::gui_GrabRounding),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_GrabRounding);

		ConfigFileValue gui_TabRounding(
			"gui_TabRounding",
			to_string(EngineGUI::gui_TabRounding),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_TabRounding);

		ConfigFileValue gui_TabBorderSize(
			"gui_TabBorderSize",
			to_string(EngineGUI::gui_TabBorderSize),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_TabBorderSize);

		ConfigFileValue gui_TabMinWidthForCloseButton(
			"gui_TabMinWidthForCloseButton",
			to_string(EngineGUI::gui_TabMinWidthForCloseButton),
			"0.0",
			"10.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_TabMinWidthForCloseButton);

		ConfigFileValue gui_ColorButtonPosition(
			"gui_ColorButtonPosition",
			to_string(EngineGUI::gui_ColorButtonPosition),
			imgui_dir,
			ConfigFileValue::Type::type_string);
		AddValue(gui_ColorButtonPosition);

		string gui_ButtonTextAlign_value =
			to_string(EngineGUI::gui_ButtonTextAlign.x) + ", " +
			to_string(EngineGUI::gui_ButtonTextAlign.y);
		ConfigFileValue gui_ButtonTextAlign(
			"gui_ButtonTextAlign",
			gui_ButtonTextAlign_value,
			"0.0, 0.0",
			"1.0, 1.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_ButtonTextAlign);

		string gui_SelectableTextAlign_value =
			to_string(EngineGUI::gui_SelectableTextAlign.x) + ", " +
			to_string(EngineGUI::gui_SelectableTextAlign.y);
		ConfigFileValue gui_SelectableTextAlign(
			"gui_SelectableTextAlign",
			gui_SelectableTextAlign_value,
			"0.0, 0.0",
			"1.0, 1.0",
			ConfigFileValue::Type::type_imvec2);
		AddValue(gui_SelectableTextAlign);

		// GUI COLORS

		string gui_Color_Text_value =
			to_string(EngineGUI::gui_Color_Text.x) + ", " +
			to_string(EngineGUI::gui_Color_Text.y) + ", " +
			to_string(EngineGUI::gui_Color_Text.z) + ", " +
			to_string(EngineGUI::gui_Color_Text.w);
		ConfigFileValue gui_Color_Text(
			"gui_Color_Text",
			gui_Color_Text_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Text);

		string gui_Color_TextDisabled_value =
			to_string(EngineGUI::gui_Color_TextDisabled.x) + ", " +
			to_string(EngineGUI::gui_Color_TextDisabled.y) + ", " +
			to_string(EngineGUI::gui_Color_TextDisabled.z) + ", " +
			to_string(EngineGUI::gui_Color_TextDisabled.w);
		ConfigFileValue gui_Color_TextDisabled(
			"gui_Color_TextDisabled",
			gui_Color_TextDisabled_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TextDisabled);

		string gui_Color_WindowBg_value =
			to_string(EngineGUI::gui_Color_WindowBg.x) + ", " +
			to_string(EngineGUI::gui_Color_WindowBg.y) + ", " +
			to_string(EngineGUI::gui_Color_WindowBg.z) + ", " +
			to_string(EngineGUI::gui_Color_WindowBg.w);
		ConfigFileValue gui_Color_WindowBg(
			"gui_Color_WindowBg",
			gui_Color_WindowBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_WindowBg);

		string gui_Color_ChildBg_value =
			to_string(EngineGUI::gui_Color_ChildBg.x) + ", " +
			to_string(EngineGUI::gui_Color_ChildBg.y) + ", " +
			to_string(EngineGUI::gui_Color_ChildBg.z) + ", " +
			to_string(EngineGUI::gui_Color_ChildBg.w);
		ConfigFileValue gui_Color_ChildBg(
			"gui_Color_ChildBg",
			gui_Color_ChildBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ChildBg);

		string gui_Color_PopupBg_value =
			to_string(EngineGUI::gui_Color_PopupBg.x) + ", " +
			to_string(EngineGUI::gui_Color_PopupBg.y) + ", " +
			to_string(EngineGUI::gui_Color_PopupBg.z) + ", " +
			to_string(EngineGUI::gui_Color_PopupBg.w);
		ConfigFileValue gui_Color_PopupBg(
			"gui_Color_PopupBg",
			gui_Color_PopupBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PopupBg);

		string gui_Color_Border_value =
			to_string(EngineGUI::gui_Color_Border.x) + ", " +
			to_string(EngineGUI::gui_Color_Border.y) + ", " +
			to_string(EngineGUI::gui_Color_Border.z) + ", " +
			to_string(EngineGUI::gui_Color_Border.w);
		ConfigFileValue gui_Color_Border(
			"gui_Color_Border",
			gui_Color_Border_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Border);

		string gui_Color_BorderShadow_value =
			to_string(EngineGUI::gui_Color_BorderShadow.x) + ", " +
			to_string(EngineGUI::gui_Color_BorderShadow.y) + ", " +
			to_string(EngineGUI::gui_Color_BorderShadow.z) + ", " +
			to_string(EngineGUI::gui_Color_BorderShadow.w);
		ConfigFileValue gui_Color_BorderShadow(
			"gui_Color_BorderShadow",
			gui_Color_BorderShadow_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_BorderShadow);

		string gui_Color_FrameBg_value =
			to_string(EngineGUI::gui_Color_FrameBg.x) + ", " +
			to_string(EngineGUI::gui_Color_FrameBg.y) + ", " +
			to_string(EngineGUI::gui_Color_FrameBg.z) + ", " +
			to_string(EngineGUI::gui_Color_FrameBg.w);
		ConfigFileValue gui_Color_FrameBg(
			"gui_Color_FrameBg",
			gui_Color_FrameBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_FrameBg);

		string gui_Color_FrameBgHovered_value =
			to_string(EngineGUI::gui_Color_FrameBgHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_FrameBgHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_FrameBgHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_FrameBgHovered.w);
		ConfigFileValue gui_Color_FrameBgHovered(
			"gui_Color_FrameBgHovered",
			gui_Color_FrameBgHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_FrameBgHovered);

		string gui_Color_FrameBgActive_value =
			to_string(EngineGUI::gui_Color_FrameBgActive.x) + ", " +
			to_string(EngineGUI::gui_Color_FrameBgActive.y) + ", " +
			to_string(EngineGUI::gui_Color_FrameBgActive.z) + ", " +
			to_string(EngineGUI::gui_Color_FrameBgActive.w);
		ConfigFileValue gui_Color_FrameBgActive(
			"gui_Color_FrameBgActive",
			gui_Color_FrameBgActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_FrameBgActive);

		string gui_Color_TitleBg_value =
			to_string(EngineGUI::gui_Color_TitleBg.x) + ", " +
			to_string(EngineGUI::gui_Color_TitleBg.y) + ", " +
			to_string(EngineGUI::gui_Color_TitleBg.z) + ", " +
			to_string(EngineGUI::gui_Color_TitleBg.w);
		ConfigFileValue gui_Color_TitleBg(
			"gui_Color_TitleBg",
			gui_Color_TitleBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TitleBg);

		string gui_Color_TitleBgActive_value =
			to_string(EngineGUI::gui_Color_TitleBgActive.x) + ", " +
			to_string(EngineGUI::gui_Color_TitleBgActive.y) + ", " +
			to_string(EngineGUI::gui_Color_TitleBgActive.z) + ", " +
			to_string(EngineGUI::gui_Color_TitleBgActive.w);
		ConfigFileValue gui_Color_TitleBgActive(
			"gui_Color_TitleBgActive",
			gui_Color_TitleBgActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TitleBgActive);

		string gui_Color_TitleBgCollapsed_value =
			to_string(EngineGUI::gui_Color_TitleBgCollapsed.x) + ", " +
			to_string(EngineGUI::gui_Color_TitleBgCollapsed.y) + ", " +
			to_string(EngineGUI::gui_Color_TitleBgCollapsed.z) + ", " +
			to_string(EngineGUI::gui_Color_TitleBgCollapsed.w);
		ConfigFileValue gui_Color_TitleBgCollapsed(
			"gui_Color_TitleBgCollapsed",
			gui_Color_TitleBgCollapsed_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TitleBgCollapsed);

		string gui_Color_MenuBarBg_value =
			to_string(EngineGUI::gui_Color_MenuBarBg.x) + ", " +
			to_string(EngineGUI::gui_Color_MenuBarBg.y) + ", " +
			to_string(EngineGUI::gui_Color_MenuBarBg.z) + ", " +
			to_string(EngineGUI::gui_Color_MenuBarBg.w);
		ConfigFileValue gui_Color_MenuBarBg(
			"gui_Color_MenuBarBg",
			gui_Color_MenuBarBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_MenuBarBg);

		string gui_Color_ScrollbarBg_value =
			to_string(EngineGUI::gui_Color_ScrollbarBg.x) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarBg.y) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarBg.z) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarBg.w);
		ConfigFileValue gui_Color_ScrollbarBg(
			"gui_Color_ScrollbarBg",
			gui_Color_ScrollbarBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ScrollbarBg);

		string gui_Color_ScrollbarGrab_value =
			to_string(EngineGUI::gui_Color_ScrollbarGrab.x) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrab.y) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrab.z) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrab.w);
		ConfigFileValue gui_Color_ScrollbarGrab(
			"gui_Color_ScrollbarGrab",
			gui_Color_ScrollbarGrab_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ScrollbarGrab);

		string gui_Color_ScrollbarGrabHovered_value =
			to_string(EngineGUI::gui_Color_ScrollbarGrabHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrabHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrabHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrabHovered.w);
		ConfigFileValue gui_Color_ScrollbarGrabHovered(
			"gui_Color_ScrollbarGrabHovered",
			gui_Color_ScrollbarGrabHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ScrollbarGrabHovered);

		string gui_Color_ScrollbarGrabActive_value =
			to_string(EngineGUI::gui_Color_ScrollbarGrabActive.x) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrabActive.y) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrabActive.z) + ", " +
			to_string(EngineGUI::gui_Color_ScrollbarGrabActive.w);
		ConfigFileValue gui_Color_ScrollbarGrabActive(
			"gui_Color_ScrollbarGrabActive",
			gui_Color_ScrollbarGrabActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ScrollbarGrabActive);

		string gui_Color_CheckMark_value =
			to_string(EngineGUI::gui_Color_CheckMark.x) + ", " +
			to_string(EngineGUI::gui_Color_CheckMark.y) + ", " +
			to_string(EngineGUI::gui_Color_CheckMark.z) + ", " +
			to_string(EngineGUI::gui_Color_CheckMark.w);
		ConfigFileValue gui_Color_CheckMark(
			"gui_Color_CheckMark",
			gui_Color_CheckMark_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_CheckMark);

		string gui_Color_SliderGrab_value =
			to_string(EngineGUI::gui_Color_SliderGrab.x) + ", " +
			to_string(EngineGUI::gui_Color_SliderGrab.y) + ", " +
			to_string(EngineGUI::gui_Color_SliderGrab.z) + ", " +
			to_string(EngineGUI::gui_Color_SliderGrab.w);
		ConfigFileValue gui_Color_SliderGrab(
			"gui_Color_SliderGrab",
			gui_Color_SliderGrab_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_SliderGrab);

		string gui_Color_SliderGrabActive_value =
			to_string(EngineGUI::gui_Color_SliderGrabActive.x) + ", " +
			to_string(EngineGUI::gui_Color_SliderGrabActive.y) + ", " +
			to_string(EngineGUI::gui_Color_SliderGrabActive.z) + ", " +
			to_string(EngineGUI::gui_Color_SliderGrabActive.w);
		ConfigFileValue gui_Color_SliderGrabActive(
			"gui_Color_SliderGrabActive",
			gui_Color_SliderGrabActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_SliderGrabActive);

		string gui_Color_Button_value =
			to_string(EngineGUI::gui_Color_Button.x) + ", " +
			to_string(EngineGUI::gui_Color_Button.y) + ", " +
			to_string(EngineGUI::gui_Color_Button.z) + ", " +
			to_string(EngineGUI::gui_Color_Button.w);
		ConfigFileValue gui_Color_Button(
			"gui_Color_Button",
			gui_Color_Button_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Button);

		string gui_Color_ButtonHovered_value =
			to_string(EngineGUI::gui_Color_ButtonHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_ButtonHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_ButtonHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_ButtonHovered.w);
		ConfigFileValue gui_Color_ButtonHovered(
			"gui_Color_ButtonHovered",
			gui_Color_ButtonHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ButtonHovered);

		string gui_Color_ButtonActive_value =
			to_string(EngineGUI::gui_Color_ButtonActive.x) + ", " +
			to_string(EngineGUI::gui_Color_ButtonActive.y) + ", " +
			to_string(EngineGUI::gui_Color_ButtonActive.z) + ", " +
			to_string(EngineGUI::gui_Color_ButtonActive.w);
		ConfigFileValue gui_Color_ButtonActive(
			"gui_Color_ButtonActive",
			gui_Color_ButtonActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ButtonActive);

		string gui_Color_Header_value =
			to_string(EngineGUI::gui_Color_Header.x) + ", " +
			to_string(EngineGUI::gui_Color_Header.y) + ", " +
			to_string(EngineGUI::gui_Color_Header.z) + ", " +
			to_string(EngineGUI::gui_Color_Header.w);
		ConfigFileValue gui_Color_Header(
			"gui_Color_Header",
			gui_Color_Header_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Header);

		string gui_Color_HeaderHovered_value =
			to_string(EngineGUI::gui_Color_HeaderHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_HeaderHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_HeaderHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_HeaderHovered.w);
		ConfigFileValue gui_Color_HeaderHovered(
			"gui_Color_HeaderHovered",
			gui_Color_HeaderHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_HeaderHovered);

		string gui_Color_HeaderActive_value =
			to_string(EngineGUI::gui_Color_HeaderActive.x) + ", " +
			to_string(EngineGUI::gui_Color_HeaderActive.y) + ", " +
			to_string(EngineGUI::gui_Color_HeaderActive.z) + ", " +
			to_string(EngineGUI::gui_Color_HeaderActive.w);
		ConfigFileValue gui_Color_HeaderActive(
			"gui_Color_HeaderActive",
			gui_Color_HeaderActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_HeaderActive);

		string gui_Color_Separator_value =
			to_string(EngineGUI::gui_Color_Separator.x) + ", " +
			to_string(EngineGUI::gui_Color_Separator.y) + ", " +
			to_string(EngineGUI::gui_Color_Separator.z) + ", " +
			to_string(EngineGUI::gui_Color_Separator.w);
		ConfigFileValue gui_Color_Separator(
			"gui_Color_Separator",
			gui_Color_Separator_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Separator);

		string gui_Color_SeparatorHovered_value =
			to_string(EngineGUI::gui_Color_SeparatorHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_SeparatorHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_SeparatorHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_SeparatorHovered.w);
		ConfigFileValue gui_Color_SeparatorHovered(
			"gui_Color_SeparatorHovered",
			gui_Color_SeparatorHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_SeparatorHovered);

		string gui_Color_SeparatorActive_value =
			to_string(EngineGUI::gui_Color_SeparatorActive.x) + ", " +
			to_string(EngineGUI::gui_Color_SeparatorActive.y) + ", " +
			to_string(EngineGUI::gui_Color_SeparatorActive.z) + ", " +
			to_string(EngineGUI::gui_Color_SeparatorActive.w);
		ConfigFileValue gui_Color_SeparatorActive(
			"gui_Color_SeparatorActive",
			gui_Color_SeparatorActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_SeparatorActive);

		string gui_Color_ResizeGrip_value =
			to_string(EngineGUI::gui_Color_ResizeGrip.x) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGrip.y) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGrip.z) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGrip.w);
		ConfigFileValue gui_Color_ResizeGrip(
			"gui_Color_ResizeGrip",
			gui_Color_ResizeGrip_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ResizeGrip);

		string gui_Color_ResizeGripHovered_value =
			to_string(EngineGUI::gui_Color_ResizeGripHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGripHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGripHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGripHovered.w);
		ConfigFileValue gui_Color_ResizeGripHovered(
			"gui_Color_ResizeGripHovered",
			gui_Color_ResizeGripHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ResizeGripHovered);

		string gui_Color_ResizeGripActive_value =
			to_string(EngineGUI::gui_Color_ResizeGripActive.x) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGripActive.y) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGripActive.z) + ", " +
			to_string(EngineGUI::gui_Color_ResizeGripActive.w);
		ConfigFileValue gui_Color_ResizeGripActive(
			"gui_Color_ResizeGripActive",
			gui_Color_ResizeGripActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ResizeGripActive);

		string gui_Color_Tab_value =
			to_string(EngineGUI::gui_Color_Tab.x) + ", " +
			to_string(EngineGUI::gui_Color_Tab.y) + ", " +
			to_string(EngineGUI::gui_Color_Tab.z) + ", " +
			to_string(EngineGUI::gui_Color_Tab.w);
		ConfigFileValue gui_Color_Tab(
			"gui_Color_Tab",
			gui_Color_Tab_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_Tab);

		string gui_Color_TabHovered_value =
			to_string(EngineGUI::gui_Color_TabHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_TabHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_TabHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_TabHovered.w);
		ConfigFileValue gui_Color_TabHovered(
			"gui_Color_TabHovered",
			gui_Color_TabHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TabHovered);

		string gui_Color_TabActive_value =
			to_string(EngineGUI::gui_Color_TabActive.x) + ", " +
			to_string(EngineGUI::gui_Color_TabActive.y) + ", " +
			to_string(EngineGUI::gui_Color_TabActive.z) + ", " +
			to_string(EngineGUI::gui_Color_TabActive.w);
		ConfigFileValue gui_Color_TabActive(
			"gui_Color_TabActive",
			gui_Color_TabActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TabActive);

		string gui_Color_TabUnfocused_value =
			to_string(EngineGUI::gui_Color_TabUnfocused.x) + ", " +
			to_string(EngineGUI::gui_Color_TabUnfocused.y) + ", " +
			to_string(EngineGUI::gui_Color_TabUnfocused.z) + ", " +
			to_string(EngineGUI::gui_Color_TabUnfocused.w);
		ConfigFileValue gui_Color_TabUnfocused(
			"gui_Color_TabUnfocused",
			gui_Color_TabUnfocused_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TabUnfocused);

		string gui_Color_TabUnfocusedActive_value =
			to_string(EngineGUI::gui_Color_TabUnfocusedActive.x) + ", " +
			to_string(EngineGUI::gui_Color_TabUnfocusedActive.y) + ", " +
			to_string(EngineGUI::gui_Color_TabUnfocusedActive.z) + ", " +
			to_string(EngineGUI::gui_Color_TabUnfocusedActive.w);
		ConfigFileValue gui_Color_TabUnfocusedActive(
			"gui_Color_TabUnfocusedActive",
			gui_Color_TabUnfocusedActive_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TabUnfocusedActive);

		string gui_Color_PlotLines_value =
			to_string(EngineGUI::gui_Color_PlotLines.x) + ", " +
			to_string(EngineGUI::gui_Color_PlotLines.y) + ", " +
			to_string(EngineGUI::gui_Color_PlotLines.z) + ", " +
			to_string(EngineGUI::gui_Color_PlotLines.w);
		ConfigFileValue gui_Color_PlotLines(
			"gui_Color_PlotLines",
			gui_Color_PlotLines_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PlotLines);

		string gui_Color_PlotLinesHovered_value =
			to_string(EngineGUI::gui_Color_PlotLinesHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_PlotLinesHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_PlotLinesHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_PlotLinesHovered.w);
		ConfigFileValue gui_Color_PlotLinesHovered(
			"gui_Color_PlotLinesHovered",
			gui_Color_PlotLinesHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PlotLinesHovered);

		string gui_Color_PlotHistogram_value =
			to_string(EngineGUI::gui_Color_PlotHistogram.x) + ", " +
			to_string(EngineGUI::gui_Color_PlotHistogram.y) + ", " +
			to_string(EngineGUI::gui_Color_PlotHistogram.z) + ", " +
			to_string(EngineGUI::gui_Color_PlotHistogram.w);
		ConfigFileValue gui_Color_PlotHistogram(
			"gui_Color_PlotHistogram",
			gui_Color_PlotHistogram_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PlotHistogram);

		string gui_Color_PlotHistogramHovered_value =
			to_string(EngineGUI::gui_Color_PlotHistogramHovered.x) + ", " +
			to_string(EngineGUI::gui_Color_PlotHistogramHovered.y) + ", " +
			to_string(EngineGUI::gui_Color_PlotHistogramHovered.z) + ", " +
			to_string(EngineGUI::gui_Color_PlotHistogramHovered.w);
		ConfigFileValue gui_Color_PlotHistogramHovered(
			"gui_Color_PlotHistogramHovered",
			gui_Color_PlotHistogramHovered_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_PlotHistogramHovered);

		string gui_Color_TableHeaderBg_value =
			to_string(EngineGUI::gui_Color_TableHeaderBg.x) + ", " +
			to_string(EngineGUI::gui_Color_TableHeaderBg.y) + ", " +
			to_string(EngineGUI::gui_Color_TableHeaderBg.z) + ", " +
			to_string(EngineGUI::gui_Color_TableHeaderBg.w);
		ConfigFileValue gui_Color_TableHeaderBg(
			"gui_Color_TableHeaderBg",
			gui_Color_TableHeaderBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableHeaderBg);

		string gui_Color_TableBorderStrong_value =
			to_string(EngineGUI::gui_Color_TableBorderStrong.x) + ", " +
			to_string(EngineGUI::gui_Color_TableBorderStrong.y) + ", " +
			to_string(EngineGUI::gui_Color_TableBorderStrong.z) + ", " +
			to_string(EngineGUI::gui_Color_TableBorderStrong.w);
		ConfigFileValue gui_Color_TableBorderStrong(
			"gui_Color_TableBorderStrong",
			gui_Color_TableBorderStrong_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableBorderStrong);

		string gui_Color_TableBorderLight_value =
			to_string(EngineGUI::gui_Color_TableBorderLight.x) + ", " +
			to_string(EngineGUI::gui_Color_TableBorderLight.y) + ", " +
			to_string(EngineGUI::gui_Color_TableBorderLight.z) + ", " +
			to_string(EngineGUI::gui_Color_TableBorderLight.w);
		ConfigFileValue gui_Color_TableBorderLight(
			"gui_Color_TableBorderLight",
			gui_Color_TableBorderLight_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableBorderLight);

		string gui_Color_TableRowBg_value =
			to_string(EngineGUI::gui_Color_TableRowBg.x) + ", " +
			to_string(EngineGUI::gui_Color_TableRowBg.y) + ", " +
			to_string(EngineGUI::gui_Color_TableRowBg.z) + ", " +
			to_string(EngineGUI::gui_Color_TableRowBg.w);
		ConfigFileValue gui_Color_TableRowBg(
			"gui_Color_TableRowBg",
			gui_Color_TableRowBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableRowBg);

		string gui_Color_TableRowBgAlt_value =
			to_string(EngineGUI::gui_Color_TableRowBgAlt.x) + ", " +
			to_string(EngineGUI::gui_Color_TableRowBgAlt.y) + ", " +
			to_string(EngineGUI::gui_Color_TableRowBgAlt.z) + ", " +
			to_string(EngineGUI::gui_Color_TableRowBgAlt.w);
		ConfigFileValue gui_Color_TableRowBgAlt(
			"gui_Color_TableRowBgAlt",
			gui_Color_TableRowBgAlt_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TableRowBgAlt);

		string gui_Color_TextSelectedBg_value =
			to_string(EngineGUI::gui_Color_TextSelectedBg.x) + ", " +
			to_string(EngineGUI::gui_Color_TextSelectedBg.y) + ", " +
			to_string(EngineGUI::gui_Color_TextSelectedBg.z) + ", " +
			to_string(EngineGUI::gui_Color_TextSelectedBg.w);
		ConfigFileValue gui_Color_TextSelectedBg(
			"gui_Color_TextSelectedBg",
			gui_Color_TextSelectedBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_TextSelectedBg);

		string gui_Color_DragDropTarget_value =
			to_string(EngineGUI::gui_Color_DragDropTarget.x) + ", " +
			to_string(EngineGUI::gui_Color_DragDropTarget.y) + ", " +
			to_string(EngineGUI::gui_Color_DragDropTarget.z) + ", " +
			to_string(EngineGUI::gui_Color_DragDropTarget.w);
		ConfigFileValue gui_Color_DragDropTarget(
			"gui_Color_DragDropTarget",
			gui_Color_DragDropTarget_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_DragDropTarget);

		string gui_Color_NavHighlight_value =
			to_string(EngineGUI::gui_Color_NavHighlight.x) + ", " +
			to_string(EngineGUI::gui_Color_NavHighlight.y) + ", " +
			to_string(EngineGUI::gui_Color_NavHighlight.z) + ", " +
			to_string(EngineGUI::gui_Color_NavHighlight.w);
		ConfigFileValue gui_Color_NavHighlight(
			"gui_Color_NavHighlight",
			gui_Color_NavHighlight_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_NavHighlight);

		string gui_Color_NavWindowingHighlight_value =
			to_string(EngineGUI::gui_Color_NavWindowingHighlight.x) + ", " +
			to_string(EngineGUI::gui_Color_NavWindowingHighlight.y) + ", " +
			to_string(EngineGUI::gui_Color_NavWindowingHighlight.z) + ", " +
			to_string(EngineGUI::gui_Color_NavWindowingHighlight.w);
		ConfigFileValue gui_Color_NavWindowingHighlight(
			"gui_Color_NavWindowingHighlight",
			gui_Color_NavWindowingHighlight_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_NavWindowingHighlight);

		string gui_Color_NavWindowingDimBg_value =
			to_string(EngineGUI::gui_Color_NavWindowingDimBg.x) + ", " +
			to_string(EngineGUI::gui_Color_NavWindowingDimBg.y) + ", " +
			to_string(EngineGUI::gui_Color_NavWindowingDimBg.z) + ", " +
			to_string(EngineGUI::gui_Color_NavWindowingDimBg.w);
		ConfigFileValue gui_Color_NavWindowingDimBg(
			"gui_Color_NavWindowingDimBg",
			gui_Color_NavWindowingDimBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_NavWindowingDimBg);

		string gui_Color_ModalWindowDimBg_value =
			to_string(EngineGUI::gui_Color_ModalWindowDimBg.x) + ", " +
			to_string(EngineGUI::gui_Color_ModalWindowDimBg.y) + ", " +
			to_string(EngineGUI::gui_Color_ModalWindowDimBg.z) + ", " +
			to_string(EngineGUI::gui_Color_ModalWindowDimBg.w);
		ConfigFileValue gui_Color_ModalWindowDimBg(
			"gui_Color_ModalWindowDimBg",
			gui_Color_ModalWindowDimBg_value,
			"0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f, 0.0000000000000001f",
			"1.0, 1.0, 1.0, 1.0",
			ConfigFileValue::Type::type_imvec4);
		AddValue(gui_Color_ModalWindowDimBg);

		//
		// GUI STYLE END
		//
	}

	void ConfigFileManager::LoadConfigFile()
	{
		//always set config file values to default first 
		//to ensure missing or invalid values are not gonna crash the engine
		SetDefaultConfigValues();

		//create new config file if it does not exist
		if (configFileManager.configFilePath == "")
		{
			configFileManager.configFilePath = Engine::docsPath + "/config.txt";

			if (!exists(configFileManager.configFilePath))
			{
				configFileManager.CreateNewConfigFile();
			}
		}

		ifstream configFile(configFileManager.configFilePath);

		if (!configFile)
		{
			string title = "Failed to open config file";
			string description = "Couldn't open config file at " + configFileManager.configFilePath + "!";
			Engine::CreateErrorPopup(title.c_str(), description.c_str());
		}

		string line;

		ConfigFileValue::Type type{};

		//variable names
		map<string, variant<bool, string, float, int, vec2, vec3, ImVec2, ImVec4>> gui_ValueNames;
		for (const auto& value : ConfigFileManager::values)
		{
			gui_ValueNames[value.GetName()] = value.GetValue();
		}

		//variable default values
		map<string, variant<bool, string, float, int, vec2, vec3, ImVec2, ImVec4>> gui_DefaultValues;
		for (const auto& value : ConfigFileManager::values)
		{
			gui_ValueNames[value.GetName()] = value.GetDefaultValue();
		}

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

				if (gui_ValueNames.find(name) == gui_ValueNames.end())
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Value " + name + " does not exist in config settings!");
					return;
				}

				static bool isInRange = false;
				switch (type)
				{
				case ConfigFileValue::Type::type_float:
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						isInRange = true;
						gui_ValueNames[name] = stof(lineVariables[0]);
					}
					break;
				case ConfigFileValue::Type::type_int:
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						isInRange = true;
						gui_ValueNames[name] = stoi(lineVariables[0]);
					}
					break;
				case ConfigFileValue::Type::type_vec2:
					if (configFileManager.IsValueInRange(name + "X", lineVariables[0])
						&& configFileManager.IsValueInRange(name + "Y", lineVariables[1]))
					{
						isInRange = true;
					}
					break;
				case ConfigFileValue::Type::type_vec3:
					if (configFileManager.IsValueInRange(name + "X", lineVariables[0])
						&& configFileManager.IsValueInRange(name + "Y", lineVariables[1])
						&& configFileManager.IsValueInRange(name + "Z", lineVariables[2]))
					{
						isInRange = true;
					}
					break;
				}

				if (isInRange)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set " + name + " to " + lineSplit[1] + ".\n");
				}
				else 
				{
					gui_ValueNames[name] = gui_DefaultValues[name];

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						name + " value " + lineVariables[0] + " is out of range or not a valid value for the current value type! Resetting to default.\n");
				}
			}
		}

		configFile.close();

		//assign specific values
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

		configFileManager.UpdateValues();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully loaded config file!\n");
	}

	void ConfigFileManager::SaveConfigFile()
	{
		if (exists(configFileManager.configFilePath))
		{
			remove(configFileManager.configFilePath);
		}
		configFileManager.UpdateValues();
		configFileManager.CreateNewConfigFile();

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
		const string& name,
		const string& value)
	{
		ConfigFileValue::Type type{};

		float currentFloatValue{}, minFloatValue{}, maxFloatValue{};
		int currentIntValue{}, minIntValue{}, maxIntValue{};
		vec2 currentVec2Value{}, minVec2Value{}, maxVec2Value{};
		vec3 currentVec3Value{}, minVec3Value{}, maxVec3Value{};
		ImVec2 currentImvec2Value{}, minImvec2Value{}, maxImvec2Value{};
		ImVec4 currentImvec4Value{}, minImvec4Value{}, maxImvec4Value{};

		for (auto& configFileValue : values)
		{
			if (configFileValue.GetName() == name)
			{
				type = configFileValue.GetType();

				if (type == ConfigFileValue::Type::type_float)
				{
					currentFloatValue = stof(configFileValue.GetValue());
					minFloatValue = stof(configFileValue.GetMinValue());
					maxFloatValue = stof(configFileValue.GetMaxValue());
				}
				else if (type == ConfigFileValue::Type::type_int)
				{
					currentIntValue = stoi(configFileValue.GetValue());
					minIntValue = stoi(configFileValue.GetMinValue());
					maxIntValue = stoi(configFileValue.GetMaxValue());
				}
				else if (type == ConfigFileValue::Type::type_vec2)
				{
					vector<string> currentSplitValue = String::Split(configFileValue.GetValue(), ',');
					currentVec2Value = vec2(stof(currentSplitValue[0]), stof(currentSplitValue[1]));

					vector<string> minSplitValue = String::Split(configFileValue.GetMinValue(), ',');
					minVec2Value = vec2(stof(minSplitValue[0]), stof(minSplitValue[1]));

					vector<string> maxSplitValue = String::Split(configFileValue.GetMaxValue(), ',');
					maxVec2Value = vec2(stof(maxSplitValue[0]), stof(maxSplitValue[1]));
				}
				else if (type == ConfigFileValue::Type::type_vec3)
				{
					vector<string> currentSplitValue = String::Split(configFileValue.GetValue(), ',');
					currentVec3Value = vec3(stof(currentSplitValue[0]), stof(currentSplitValue[1]), stof(currentSplitValue[2]));

					vector<string> minSplitValue = String::Split(configFileValue.GetMinValue(), ',');
					minVec3Value = vec3(stof(minSplitValue[0]), stof(minSplitValue[1]), stof(minSplitValue[2]));

					vector<string> maxSplitValue = String::Split(configFileValue.GetMaxValue(), ',');
					maxVec3Value = vec3(stof(maxSplitValue[0]), stof(maxSplitValue[1]), stof(maxSplitValue[2]));
				}
				else if (type == ConfigFileValue::Type::type_imvec2)
				{
					vector<string> currentSplitValue = String::Split(configFileValue.GetValue(), ',');
					currentImvec2Value = ImVec2(stof(currentSplitValue[0]), stof(currentSplitValue[1]));

					vector<string> minSplitValue = String::Split(configFileValue.GetMinValue(), ',');
					minImvec2Value = ImVec2(stof(minSplitValue[0]), stof(minSplitValue[1]));

					vector<string> maxSplitValue = String::Split(configFileValue.GetMaxValue(), ',');
					maxImvec2Value = ImVec2(stof(maxSplitValue[0]), stof(maxSplitValue[1]));
				}
				else if (type == ConfigFileValue::Type::type_imvec4)
				{
					vector<string> currentSplitValue = String::Split(configFileValue.GetValue(), ',');
					currentImvec4Value = ImVec4(
						stof(currentSplitValue[0]), 
						stof(currentSplitValue[1]), 
						stof(currentSplitValue[2]), 
						stof(currentSplitValue[3]));

					vector<string> minSplitValue = String::Split(configFileValue.GetMinValue(), ',');
					minImvec4Value = ImVec4(
						stof(minSplitValue[0]), 
						stof(minSplitValue[1]), 
						stof(minSplitValue[2]), 
						stof(minSplitValue[3]));

					vector<string> maxSplitValue = String::Split(configFileValue.GetMaxValue(), ',');
					maxImvec4Value = ImVec4(
						stof(maxSplitValue[0]), 
						stof(maxSplitValue[1]), 
						stof(maxSplitValue[2]), 
						stof(maxSplitValue[3]));
				}

				break;
			}
		}

		bool isCorrectType = false;
		vec2 vec2Value{};
		vec3 vec3Value{};
		ImVec2 imvec2Value{};
		ImVec4 imvec4Value{};

		switch (type)
		{
		case ConfigFileValue::Type::type_string:
			if (name == "gui_WindowMenuButtonPosition"
				|| name == "gui_ColorButtonPosition")
			{
				return find(imgui_dir.begin(), imgui_dir.end(), value) != imgui_dir.end();
			}
			else return false;
		case ConfigFileValue::Type::type_float:
			isCorrectType = String::CanConvertStringToFloat(value);
			break;
		case ConfigFileValue::Type::type_int:
			isCorrectType = String::CanConvertStringToInt(value);
			break;
		case ConfigFileValue::Type::type_vec2:
			if (String::ContainsString(value, ", "))
			{
				string newValue = String::StringReplace(value, ", ", ",");
				vector<string> splitVec2 = String::Split(newValue, ',');
				vec2Value = vec2(stof(splitVec2[0]), stof(splitVec2[1]));
			}
			else return false;
			break;
		case ConfigFileValue::Type::type_vec3:
			if (String::ContainsString(value, ", "))
			{
				string newValue = String::StringReplace(value, ", ", ",");
				vector<string> splitVec3 = String::Split(newValue, ',');
				vec3Value = vec3(stof(splitVec3[0]), stof(splitVec3[1]), stof(splitVec3[2]));
			}
			break;
		case ConfigFileValue::Type::type_imvec2:
			if (String::ContainsString(value, ", "))
			{
				string newValue = String::StringReplace(value, ", ", ",");
				vector<string> splitImvec2 = String::Split(newValue, ',');
				imvec2Value = ImVec2(stof(splitImvec2[0]), stof(splitImvec2[1]));
			}
			break;
		case ConfigFileValue::Type::type_imvec4:
			if (String::ContainsString(value, ", "))
			{
				string newValue = String::StringReplace(value, ", ", ",");
				vector<string> splitImvec4 = String::Split(newValue, ',');
				imvec4Value = ImVec4(stof(splitImvec4[0]), stof(splitImvec4[1]), stof(splitImvec4[2]), stof(splitImvec4[3]));
			}
			break;
		}

		if (!isCorrectType) return false;

		float floatValue = stof(value);
		int intValue = stoi(value);

		if (type == ConfigFileValue::Type::type_float)
		{
			return floatValue >= minFloatValue && floatValue <= maxFloatValue;
		}
		else if (type == ConfigFileValue::Type::type_int)
		{
			return intValue >= minIntValue && intValue <= maxIntValue;
		}
		else if (type == ConfigFileValue::Type::type_vec2)
		{
			return vec2Value.x >= minVec2Value.x
				&& vec2Value.y >= minVec2Value.y
				&& vec2Value.x <= maxVec2Value.x
				&& vec2Value.y <= maxVec2Value.y;
		}
		else if (type == ConfigFileValue::Type::type_vec3)
		{
			return vec3Value.x >= minVec3Value.x
				&& vec3Value.y >= minVec3Value.y
				&& vec3Value.z >= minVec3Value.z
				&& vec3Value.x <= maxVec3Value.x
				&& vec3Value.y <= maxVec3Value.y
				&& vec3Value.z <= maxVec3Value.z;
		}
		else if (type == ConfigFileValue::Type::type_imvec2)
		{
			return imvec2Value.x >= minImvec2Value.x
				&& imvec2Value.y >= minImvec2Value.y
				&& imvec2Value.x <= maxImvec2Value.x
				&& imvec2Value.y <= maxImvec2Value.y;
		}
		else if (type == ConfigFileValue::Type::type_imvec4)
		{
			return imvec4Value.x >= minImvec4Value.x
				&& imvec4Value.y >= minImvec4Value.y
				&& imvec4Value.z >= minImvec4Value.z
				&& imvec4Value.w >= minImvec4Value.w
				&& imvec4Value.x <= maxImvec4Value.x
				&& imvec4Value.y <= maxImvec4Value.y
				&& imvec4Value.z <= maxImvec4Value.z
				&& imvec4Value.w <= maxImvec4Value.w;
		}

		return false;
	}
}