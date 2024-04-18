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

		static inline ImVec2 initialPos;
		static inline ImVec2 initialSize;

		static inline ImVec2 minSize;
		static inline ImVec2 maxSize;

		static inline bool renderUnsavedShutdownWindow;
		static inline bool renderUnsavedSceneSwitchWindow;
		static inline string targetScene;

		static void Initialize();

		static void CustomizeImGuiStyle();

		int GetScreenWidth();
		int GetScreenHeight();
		static float GetScreenRefreshRate();

		static void Render();
		static void Shutdown();
	private:
		//text filter for searching
		static inline ImGuiTextFilter textFilter;

		static inline bool filterTextProcessed;

		static void RenderTopBar();

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