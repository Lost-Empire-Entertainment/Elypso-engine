//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

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