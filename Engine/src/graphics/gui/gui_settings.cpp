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

		ImGui::Separator();

		ImGui::Text("Child rounding");
		if (ImGui::DragFloat("##gui_ChildRounding", &EngineGUI::gui_ChildRounding, 0.01f, 0.1f, 1.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.ChildRounding = EngineGUI::gui_ChildRounding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Child border size");
		if (ImGui::DragFloat("##gui_ChildBorderSize", &EngineGUI::gui_ChildBorderSize, 0.01f, 0.1f, 1.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.ChildBorderSize = EngineGUI::gui_ChildBorderSize;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Popup rounding");
		if (ImGui::DragFloat("##gui_PopupRounding", &EngineGUI::gui_PopupRounding, 0.01f, 0.1f, 1.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.PopupRounding = EngineGUI::gui_PopupRounding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Popup border size");
		if (ImGui::DragFloat("##gui_PopupBorderSize", &EngineGUI::gui_PopupBorderSize, 0.01f, 0.1f, 1.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.PopupBorderSize = EngineGUI::gui_PopupBorderSize;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

		vec2 gui_FramePadding = vec2(
			EngineGUI::gui_FramePadding.x,
			EngineGUI::gui_FramePadding.y);
		ImGui::Text("Frame padding");
		if (ImGui::DragFloat2("##gui_FramePadding", value_ptr(gui_FramePadding), 0.01f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			EngineGUI::gui_FramePadding = ImVec2(
				gui_FramePadding.x,
				gui_FramePadding.y);
			style.FramePadding = EngineGUI::gui_FramePadding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Frame rounding");
		if (ImGui::DragFloat("##gui_FrameRounding", &EngineGUI::gui_FrameRounding, 0.01f, 0.1f, 1.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.FrameRounding = EngineGUI::gui_FrameRounding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Frame border size");
		if (ImGui::DragFloat("##gui_FrameBorderSize", &EngineGUI::gui_FrameBorderSize, 0.01f, 0.1f, 1.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.FrameBorderSize = EngineGUI::gui_FrameBorderSize;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

		vec2 gui_ItemSpacing = vec2(
			EngineGUI::gui_ItemSpacing.x,
			EngineGUI::gui_ItemSpacing.y);
		ImGui::Text("Item spacing");
		if (ImGui::DragFloat2("##gui_ItemSpacing", value_ptr(gui_ItemSpacing), 0.01f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			EngineGUI::gui_ItemSpacing = ImVec2(
				gui_ItemSpacing.x,
				gui_ItemSpacing.y);
			style.ItemSpacing = EngineGUI::gui_ItemSpacing;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		vec2 gui_ItemInnerSpacing = vec2(
			EngineGUI::gui_ItemInnerSpacing.x,
			EngineGUI::gui_ItemInnerSpacing.y);
		ImGui::Text("Item inner spacing");
		if (ImGui::DragFloat2("##gui_ItemInnerSpacing", value_ptr(gui_ItemInnerSpacing), 0.01f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			EngineGUI::gui_ItemInnerSpacing = ImVec2(
				gui_ItemInnerSpacing.x,
				gui_ItemInnerSpacing.y);
			style.ItemInnerSpacing = EngineGUI::gui_ItemInnerSpacing;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

		vec2 gui_CellPadding = vec2(
			EngineGUI::gui_CellPadding.x,
			EngineGUI::gui_CellPadding.y);
		ImGui::Text("Cell padding");
		if (ImGui::DragFloat2("##gui_CellPadding", value_ptr(gui_CellPadding), 0.01f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			EngineGUI::gui_CellPadding = ImVec2(
				gui_CellPadding.x,
				gui_CellPadding.y);
			style.CellPadding = EngineGUI::gui_CellPadding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Indent spacing");
		if (ImGui::DragFloat("##gui_IndentSpacing", &EngineGUI::gui_IndentSpacing, 0.1f, 0.1f, 50.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.IndentSpacing = EngineGUI::gui_IndentSpacing;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Columns min spacing");
		if (ImGui::DragFloat("##gui_ColumnsMinSpacing", &EngineGUI::gui_ColumnsMinSpacing, 0.1f, 0.1f, 50.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.ColumnsMinSpacing = EngineGUI::gui_ColumnsMinSpacing;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

		ImGui::Text("Scrollbar size");
		if (ImGui::DragFloat("##gui_ScrollbarSize", &EngineGUI::gui_ScrollbarSize, 0.1f, 0.1f, 50.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.ScrollbarSize = EngineGUI::gui_ScrollbarSize;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Scrollbar rounding");
		if (ImGui::DragFloat("##gui_ScrollbarRounding", &EngineGUI::gui_ScrollbarRounding, 0.01f, 0.1f, 10.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.ScrollbarRounding = EngineGUI::gui_ScrollbarRounding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

		ImGui::Text("Grab min size");
		if (ImGui::DragFloat("##gui_GrabMinSize", &EngineGUI::gui_GrabMinSize, 0.1f, 0.1f, 50.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.GrabMinSize = EngineGUI::gui_GrabMinSize;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Grab rounding");
		if (ImGui::DragFloat("##gui_GrabRounding", &EngineGUI::gui_GrabRounding, 0.01f, 0.1f, 10.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.GrabRounding = EngineGUI::gui_GrabRounding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

		ImGui::Text("Tab rounding");
		if (ImGui::DragFloat("##gui_TabRounding", &EngineGUI::gui_TabRounding, 0.01f, 0.1f, 10.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.TabRounding = EngineGUI::gui_TabRounding;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Tab border size");
		if (ImGui::DragFloat("##gui_TabBorderSize", &EngineGUI::gui_TabBorderSize, 0.01f, 0.1f, 10.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.TabBorderSize = EngineGUI::gui_TabBorderSize;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Text("Tab min width for close button");
		if (ImGui::DragFloat("##gui_TabMinWidthForCloseButton", &EngineGUI::gui_TabMinWidthForCloseButton, 0.01f, 0.1f, 10.0f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.TabMinWidthForCloseButton = EngineGUI::gui_TabMinWidthForCloseButton;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

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

		vec2 gui_ButtonTextAlign = vec2(
			EngineGUI::gui_ButtonTextAlign.x,
			EngineGUI::gui_ButtonTextAlign.y);
		ImGui::Text("Button text align");
		if (ImGui::DragFloat2("##gui_ButtonTextAlign", value_ptr(gui_ButtonTextAlign), 0.01f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			EngineGUI::gui_ButtonTextAlign = ImVec2(
				gui_ButtonTextAlign.x,
				gui_ButtonTextAlign.y);
			style.ButtonTextAlign = EngineGUI::gui_ButtonTextAlign;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		vec2 gui_SelectableTextAlign = vec2(
			EngineGUI::gui_SelectableTextAlign.x,
			EngineGUI::gui_SelectableTextAlign.y);
		ImGui::Text("Selectable text align");
		if (ImGui::DragFloat2("##gui_SelectableTextAlign", value_ptr(gui_SelectableTextAlign), 0.01f))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			EngineGUI::gui_SelectableTextAlign = ImVec2(
				gui_SelectableTextAlign.x,
				gui_SelectableTextAlign.y);
			style.SelectableTextAlign = EngineGUI::gui_SelectableTextAlign;
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
	}

	void GUISettings::GUIColorSettings()
	{
		ImGui::Text("Text color");
		if (ImGui::ColorEdit4("##gui_Color_Text", (float*)&EngineGUI::gui_Color_Text))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text] = EngineGUI::gui_Color_Text;
		}

		ImGui::Text("Disabled text color");
		if (ImGui::ColorEdit4("##gui_Color_TextDisabled", (float*)&EngineGUI::gui_Color_TextDisabled))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TextDisabled] = EngineGUI::gui_Color_TextDisabled;
		}

		ImGui::Separator();

		ImGui::Text("Window bg");
		if (ImGui::ColorEdit4("##gui_Color_WindowBg", (float*)&EngineGUI::gui_Color_WindowBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_WindowBg] = EngineGUI::gui_Color_WindowBg;
		}

		ImGui::Text("Child bg");
		if (ImGui::ColorEdit4("##gui_Color_ChildBg", (float*)&EngineGUI::gui_Color_ChildBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ChildBg] = EngineGUI::gui_Color_ChildBg;
		}

		ImGui::Text("Popup bg");
		if (ImGui::ColorEdit4("##gui_Color_PopupBg", (float*)&EngineGUI::gui_Color_PopupBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_PopupBg] = EngineGUI::gui_Color_PopupBg;
		}

		ImGui::Text("Border");
		if (ImGui::ColorEdit4("##gui_Color_Border", (float*)&EngineGUI::gui_Color_Border))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Border] = EngineGUI::gui_Color_Border;
		}

		ImGui::Text("Border shadow");
		if (ImGui::ColorEdit4("##gui_Color_BorderShadow", (float*)&EngineGUI::gui_Color_BorderShadow))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_BorderShadow] = EngineGUI::gui_Color_BorderShadow;
		}

		ImGui::Separator();

		ImGui::Text("Frame bg");
		if (ImGui::ColorEdit4("##gui_Color_FrameBg", (float*)&EngineGUI::gui_Color_FrameBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_FrameBg] = EngineGUI::gui_Color_FrameBg;
		}

		ImGui::Text("Frame bg hovered");
		if (ImGui::ColorEdit4("##gui_Color_FrameBgHovered", (float*)&EngineGUI::gui_Color_FrameBgHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_FrameBgHovered] = EngineGUI::gui_Color_FrameBgHovered;
		}

		ImGui::Text("Frame bg active");
		if (ImGui::ColorEdit4("##gui_Color_FrameBgActive", (float*)&EngineGUI::gui_Color_FrameBgActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_FrameBgActive] = EngineGUI::gui_Color_FrameBgActive;
		}

		ImGui::Text("Title bg");
		if (ImGui::ColorEdit4("##gui_Color_TitleBg", (float*)&EngineGUI::gui_Color_TitleBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TitleBg] = EngineGUI::gui_Color_TitleBg;
		}

		ImGui::Text("Title bg active");
		if (ImGui::ColorEdit4("##gui_Color_TitleBgActive", (float*)&EngineGUI::gui_Color_TitleBgActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TitleBgActive] = EngineGUI::gui_Color_TitleBgActive;
		}

		ImGui::Text("Title bg collapsed");
		if (ImGui::ColorEdit4("##gui_Color_TitleBgCollapsed", (float*)&EngineGUI::gui_Color_TitleBgCollapsed))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TitleBgCollapsed] = EngineGUI::gui_Color_TitleBgCollapsed;
		}

		ImGui::Text("Menu bar bg");
		if (ImGui::ColorEdit4("##gui_Color_MenuBarBg", (float*)&EngineGUI::gui_Color_MenuBarBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_MenuBarBg] = EngineGUI::gui_Color_MenuBarBg;
		}

		ImGui::Text("Scrollbar bg");
		if (ImGui::ColorEdit4("##gui_Color_ScrollbarBg", (float*)&EngineGUI::gui_Color_ScrollbarBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ScrollbarBg] = EngineGUI::gui_Color_ScrollbarBg;
		}

		ImGui::Text("Scrollbar grab");
		if (ImGui::ColorEdit4("##gui_Color_ScrollbarGrab", (float*)&EngineGUI::gui_Color_ScrollbarGrab))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ScrollbarGrab] = EngineGUI::gui_Color_ScrollbarGrab;
		}

		ImGui::Text("Scrollbar grab hovered");
		if (ImGui::ColorEdit4("##gui_Color_ScrollbarGrabHovered", (float*)&EngineGUI::gui_Color_ScrollbarGrabHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = EngineGUI::gui_Color_ScrollbarGrabHovered;
		}

		ImGui::Text("Scrollbar grab active");
		if (ImGui::ColorEdit4("##gui_Color_ScrollbarGrabActive", (float*)&EngineGUI::gui_Color_ScrollbarGrabActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ScrollbarGrabActive] = EngineGUI::gui_Color_ScrollbarGrabActive;
		}

		ImGui::Separator();

		ImGui::Text("Checkmark");
		if (ImGui::ColorEdit4("##gui_Color_CheckMark", (float*)&EngineGUI::gui_Color_CheckMark))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_CheckMark] = EngineGUI::gui_Color_CheckMark;
		}

		ImGui::Text("Slider grab");
		if (ImGui::ColorEdit4("##gui_Color_SliderGrab", (float*)&EngineGUI::gui_Color_SliderGrab))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_SliderGrab] = EngineGUI::gui_Color_SliderGrab;
		}

		ImGui::Text("Slider grab active");
		if (ImGui::ColorEdit4("##gui_Color_SliderGrabActive", (float*)&EngineGUI::gui_Color_SliderGrabActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_SliderGrabActive] = EngineGUI::gui_Color_SliderGrabActive;
		}

		ImGui::Separator();

		ImGui::Text("Button");
		if (ImGui::ColorEdit4("##gui_Color_Button", (float*)&EngineGUI::gui_Color_Button))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Button] = EngineGUI::gui_Color_Button;
		}

		ImGui::Text("Button hovered");
		if (ImGui::ColorEdit4("##gui_Color_ButtonHovered", (float*)&EngineGUI::gui_Color_ButtonHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ButtonHovered] = EngineGUI::gui_Color_ButtonHovered;
		}

		ImGui::Text("Button active");
		if (ImGui::ColorEdit4("##gui_Color_ButtonActive", (float*)&EngineGUI::gui_Color_ButtonActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ButtonActive] = EngineGUI::gui_Color_ButtonActive;
		}

		ImGui::Separator();

		ImGui::Text("Header");
		if (ImGui::ColorEdit4("##gui_Color_Header", (float*)&EngineGUI::gui_Color_Header))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Header] = EngineGUI::gui_Color_Header;
		}

		ImGui::Text("Header hovered");
		if (ImGui::ColorEdit4("##gui_Color_HeaderHovered", (float*)&EngineGUI::gui_Color_HeaderHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_HeaderHovered] = EngineGUI::gui_Color_HeaderHovered;
		}

		ImGui::Text("Header active");
		if (ImGui::ColorEdit4("##gui_Color_HeaderActive", (float*)&EngineGUI::gui_Color_HeaderActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_HeaderActive] = EngineGUI::gui_Color_HeaderActive;
		}

		ImGui::Text("Separator");
		if (ImGui::ColorEdit4("##gui_Color_Separator", (float*)&EngineGUI::gui_Color_Separator))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Separator] = EngineGUI::gui_Color_Separator;
		}

		ImGui::Text("Separator hovered");
		if (ImGui::ColorEdit4("##gui_Color_SeparatorHovered", (float*)&EngineGUI::gui_Color_SeparatorHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_SeparatorHovered] = EngineGUI::gui_Color_SeparatorHovered;
		}

		ImGui::Text("Separator active");
		if (ImGui::ColorEdit4("##gui_Color_SeparatorActive", (float*)&EngineGUI::gui_Color_SeparatorActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_SeparatorActive] = EngineGUI::gui_Color_SeparatorActive;
		}

		ImGui::Text("Resize grip");
		if (ImGui::ColorEdit4("##gui_Color_ResizeGrip", (float*)&EngineGUI::gui_Color_ResizeGrip))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ResizeGrip] = EngineGUI::gui_Color_ResizeGrip;
		}

		ImGui::Text("Resize grip hovered");
		if (ImGui::ColorEdit4("##gui_Color_ResizeGripHovered", (float*)&EngineGUI::gui_Color_ResizeGripHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ResizeGripHovered] = EngineGUI::gui_Color_ResizeGripHovered;
		}

		ImGui::Text("Resize grip active");
		if (ImGui::ColorEdit4("##gui_Color_ResizeGripActive", (float*)&EngineGUI::gui_Color_ResizeGripActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ResizeGripActive] = EngineGUI::gui_Color_ResizeGripActive;
		}

		ImGui::Separator();

		ImGui::Text("Tab");
		if (ImGui::ColorEdit4("##gui_Color_Tab", (float*)&EngineGUI::gui_Color_Tab))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Tab] = EngineGUI::gui_Color_Tab;
		}

		ImGui::Text("Tab hovered");
		if (ImGui::ColorEdit4("##gui_Color_TabHovered", (float*)&EngineGUI::gui_Color_TabHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TabHovered] = EngineGUI::gui_Color_TabHovered;
		}

		ImGui::Text("Tab active");
		if (ImGui::ColorEdit4("##gui_Color_TabActive", (float*)&EngineGUI::gui_Color_TabActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TabActive] = EngineGUI::gui_Color_TabActive;
		}

		ImGui::Text("Tab unfocused");
		if (ImGui::ColorEdit4("##gui_Color_TabUnfocused", (float*)&EngineGUI::gui_Color_TabUnfocused))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TabUnfocused] = EngineGUI::gui_Color_TabUnfocused;
		}

		ImGui::Text("Tab unfocused active");
		if (ImGui::ColorEdit4("##gui_Color_TabUnfocusedActive", (float*)&EngineGUI::gui_Color_TabUnfocusedActive))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TabUnfocusedActive] = EngineGUI::gui_Color_TabUnfocusedActive;
		}

		ImGui::Separator();

		ImGui::Text("Plot lines");
		if (ImGui::ColorEdit4("##gui_Color_PlotLines", (float*)&EngineGUI::gui_Color_PlotLines))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_PlotLines] = EngineGUI::gui_Color_PlotLines;
		}

		ImGui::Text("Plot lines hovered");
		if (ImGui::ColorEdit4("##gui_Color_PlotLinesHovered", (float*)&EngineGUI::gui_Color_PlotLinesHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_PlotLinesHovered] = EngineGUI::gui_Color_PlotLinesHovered;
		}

		ImGui::Text("Plot histogram");
		if (ImGui::ColorEdit4("##gui_Color_PlotHistogram", (float*)&EngineGUI::gui_Color_PlotHistogram))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_PlotHistogram] = EngineGUI::gui_Color_PlotHistogram;
		}

		ImGui::Text("Plot histogram hovered");
		if (ImGui::ColorEdit4("##gui_Color_PlotHistogramHovered", (float*)&EngineGUI::gui_Color_PlotHistogramHovered))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_PlotHistogramHovered] = EngineGUI::gui_Color_PlotHistogramHovered;
		}

		ImGui::Separator();

		ImGui::Text("Table header bg");
		if (ImGui::ColorEdit4("##gui_Color_TableHeaderBg", (float*)&EngineGUI::gui_Color_TableHeaderBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TableHeaderBg] = EngineGUI::gui_Color_TableHeaderBg;
		}

		ImGui::Text("Table border strong");
		if (ImGui::ColorEdit4("##gui_Color_TableBorderStrong", (float*)&EngineGUI::gui_Color_TableBorderStrong))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TableBorderStrong] = EngineGUI::gui_Color_TableBorderStrong;
		}

		ImGui::Text("Table border light");
		if (ImGui::ColorEdit4("##gui_Color_TableBorderLight", (float*)&EngineGUI::gui_Color_TableBorderLight))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TableBorderLight] = EngineGUI::gui_Color_TableBorderLight;
		}

		ImGui::Text("Table row bg");
		if (ImGui::ColorEdit4("##gui_Color_TableRowBg", (float*)&EngineGUI::gui_Color_TableRowBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TableRowBg] = EngineGUI::gui_Color_TableRowBg;
		}

		ImGui::Text("Table row bg alt");
		if (ImGui::ColorEdit4("##gui_Color_TableRowBgAlt", (float*)&EngineGUI::gui_Color_TableRowBgAlt))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TableRowBgAlt] = EngineGUI::gui_Color_TableRowBgAlt;
		}

		ImGui::Separator();

		ImGui::Text("Text selected bg");
		if (ImGui::ColorEdit4("##gui_Color_TextSelectedBg", (float*)&EngineGUI::gui_Color_TextSelectedBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_TextSelectedBg] = EngineGUI::gui_Color_TextSelectedBg;
		}

		ImGui::Text("Drag drop target");
		if (ImGui::ColorEdit4("##gui_Color_DragDropTarget", (float*)&EngineGUI::gui_Color_DragDropTarget))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_DragDropTarget] = EngineGUI::gui_Color_DragDropTarget;
		}

		ImGui::Text("Nav highlight");
		if (ImGui::ColorEdit4("##gui_Color_NavHighlight", (float*)&EngineGUI::gui_Color_NavHighlight))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_NavHighlight] = EngineGUI::gui_Color_NavHighlight;
		}

		ImGui::Text("Nav windowing highlight");
		if (ImGui::ColorEdit4("##gui_Color_NavWindowingHighlight", (float*)&EngineGUI::gui_Color_NavWindowingHighlight))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_NavWindowingHighlight] = EngineGUI::gui_Color_NavWindowingHighlight;
		}

		ImGui::Text("Nav windowing dim bg");
		if (ImGui::ColorEdit4("##gui_Color_NavWindowingDimBg", (float*)&EngineGUI::gui_Color_NavWindowingDimBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_NavWindowingDimBg] = EngineGUI::gui_Color_NavWindowingDimBg;
		}

		ImGui::Text("Modal window dim bg");
		if (ImGui::ColorEdit4("##gui_Color_ModalWindowDimBg", (float*)&EngineGUI::gui_Color_ModalWindowDimBg))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_ModalWindowDimBg] = EngineGUI::gui_Color_ModalWindowDimBg;
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