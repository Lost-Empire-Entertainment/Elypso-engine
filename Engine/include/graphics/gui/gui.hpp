//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>

//external
#include "imgui.h"

using std::vector;
using std::string;

namespace Graphics::GUI
{
	class EngineGUI
	{
	public:
		static inline bool showVersionWindow;
		static inline bool outdatedVersion;

		static inline float fontScale;

		//
		// GUI STYLE START
		//

		static inline float gui_Alpha;
		static inline float gui_DisabledAlpha;
		static inline ImVec2 gui_WindowPadding;
		static inline float gui_WindowRounding;
		static inline float gui_WindowBorderSize;
		static inline ImGuiDir_ gui_WindowMenuButtonPosition;
		static inline float gui_ChildRounding;
		static inline float gui_ChildBorderSize;
		static inline float gui_PopupRounding;
		static inline float gui_PopupBorderSize;
		static inline ImVec2 gui_FramePadding;
		static inline float gui_FrameRounding;
		static inline float gui_FrameBorderSize;
		static inline ImVec2 gui_ItemSpacing;
		static inline ImVec2 gui_ItemInnerSpacing;
		static inline ImVec2 gui_CellPadding;
		static inline float gui_IndentSpacing;
		static inline float gui_ColumnsMinSpacing;
		static inline float gui_ScrollbarSize;
		static inline float gui_ScrollbarRounding;
		static inline float gui_GrabMinSize;
		static inline float gui_GrabRounding;
		static inline float gui_TabRounding;
		static inline float gui_TabBorderSize;
		static inline float gui_TabMinWidthForCloseButton;
		static inline ImGuiDir_ gui_ColorButtonPosition;
		static inline ImVec2 gui_ButtonTextAlign;
		static inline ImVec2 gui_SelectableTextAlign;

		static inline ImVec4 gui_Color_Text;
		static inline ImVec4 gui_Color_TextDisabled;
		static inline ImVec4 gui_Color_WindowBg;
		static inline ImVec4 gui_Color_ChildBg;
		static inline ImVec4 gui_Color_PopupBg;
		static inline ImVec4 gui_Color_Border;
		static inline ImVec4 gui_Color_BorderShadow;
		static inline ImVec4 gui_Color_FrameBg;
		static inline ImVec4 gui_Color_FrameBgHovered;
		static inline ImVec4 gui_Color_FrameBgActive;
		static inline ImVec4 gui_Color_TitleBg;
		static inline ImVec4 gui_Color_TitleBgActive;
		static inline ImVec4 gui_Color_TitleBgCollapsed;
		static inline ImVec4 gui_Color_MenuBarBg;
		static inline ImVec4 gui_Color_ScrollbarBg;
		static inline ImVec4 gui_Color_ScrollbarGrab;
		static inline ImVec4 gui_Color_ScrollbarGrabHovered;
		static inline ImVec4 gui_Color_ScrollbarGrabActive;
		static inline ImVec4 gui_Color_CheckMark;
		static inline ImVec4 gui_Color_SliderGrab;
		static inline ImVec4 gui_Color_SliderGrabActive;
		static inline ImVec4 gui_Color_Button;
		static inline ImVec4 gui_Color_ButtonHovered;
		static inline ImVec4 gui_Color_ButtonActive;
		static inline ImVec4 gui_Color_Header;
		static inline ImVec4 gui_Color_HeaderHovered;
		static inline ImVec4 gui_Color_HeaderActive;
		static inline ImVec4 gui_Color_Separator;
		static inline ImVec4 gui_Color_SeparatorHovered;
		static inline ImVec4 gui_Color_SeparatorActive;
		static inline ImVec4 gui_Color_ResizeGrip;
		static inline ImVec4 gui_Color_ResizeGripHovered;
		static inline ImVec4 gui_Color_ResizeGripActive;
		static inline ImVec4 gui_Color_Tab;
		static inline ImVec4 gui_Color_TabHovered;
		static inline ImVec4 gui_Color_TabActive;
		static inline ImVec4 gui_Color_TabUnfocused;
		static inline ImVec4 gui_Color_TabUnfocusedActive;
		static inline ImVec4 gui_Color_PlotLines;
		static inline ImVec4 gui_Color_PlotLinesHovered;
		static inline ImVec4 gui_Color_PlotHistogram;
		static inline ImVec4 gui_Color_PlotHistogramHovered;
		static inline ImVec4 gui_Color_TableHeaderBg;
		static inline ImVec4 gui_Color_TableBorderStrong;
		static inline ImVec4 gui_Color_TableBorderLight;
		static inline ImVec4 gui_Color_TableRowBg;
		static inline ImVec4 gui_Color_TableRowBgAlt;
		static inline ImVec4 gui_Color_TextSelectedBg;
		static inline ImVec4 gui_Color_DragDropTarget;
		static inline ImVec4 gui_Color_NavHighlight;
		static inline ImVec4 gui_Color_NavWindowingHighlight;
		static inline ImVec4 gui_Color_NavWindowingDimBg;
		static inline ImVec4 gui_Color_ModalWindowDimBg;

		//
		// GUI STYLE END
		//

		static inline ImVec2 initialPos;
		static inline ImVec2 initialSize;

		static inline ImVec2 minSize;
		static inline ImVec2 maxSize;

		static inline bool renderUnsavedShutdownWindow;
		static inline bool renderUnsavedSceneSwitchWindow;
		static inline string targetScene;

		static void Initialize();

		static void CustomizeImGuiStyle();

		static void Render();
		static void Shutdown();
	private:
		//text filter for searching
		static ImGuiTextFilter textFilter;

		static inline bool filterTextProcessed;

		static void RenderTopBar();

		static inline int GetScreenWidth();
		static inline int GetScreenHeight();
		static inline float GetScreenRefreshRate();

		//top bar interactions
		static void TB_CheckVersion();
		static void TB_ReportIssue();

		//top bar rendered windows
		static inline string versionCompare;
		static inline string versionConfirm;
		static void RenderVersionCheckWindow();

		static void ConfirmUnsavedShutdown();

		static void ConfirmUnsavedSceneSwitch();
	};
}