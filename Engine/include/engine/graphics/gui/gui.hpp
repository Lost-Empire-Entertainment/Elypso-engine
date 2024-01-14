//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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

//external
#include "imgui.h"

#include <vector>
#include <string>

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

		static inline ImVec2 initialWindowPos = ImVec2(200, 150);
		static inline ImVec2 minWindowSize = ImVec2(-1, 0);//(400, 400);
		static inline ImVec2 maxWindowSize;

		void Initialize();

		void CustomizeImGuiStyle();

		int GetScreenWidth();
		int GetScreenHeight();
		static float GetScreenRefreshRate();

		void Render();
		static void Shutdown();
		static EngineGUI& GetInstance();
	private:
		//text filter for searching
		static inline ImGuiTextFilter textFilter;

		static inline bool filterTextProcessed;

		void RenderTopBar();
		void RenderBottomBar();

		//top bar interactions
		void TB_CheckVersion();
		void TB_ReportIssue();

		//top bar rendered windows
		void RenderVersionCheckWindow();
		static inline string versionCompare;
		static inline string versionConfirm;
	};
}