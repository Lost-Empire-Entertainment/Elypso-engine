//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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

namespace Graphics
{
	class GUI
	{
	public:
		static inline bool writeToConsole;
		static inline bool firstScrollToBottom;
		static inline bool allowScrollToBottom;

		static inline bool showKeybindsMenu;
		static inline bool showDebugMenu;
		static inline bool showConsole;
		static inline bool showLightsMenu;
		static inline bool showVersionWindow;
		static inline bool outdatedVersion;

		static inline float fontScale;

		//holds text content
		static inline ImGuiTextBuffer textBuffer;

		void Initialize();

		void CustomizeImGuiStyle();

		int GetScreenWidth();
		int GetScreenHeight();
		static float GetScreenRefreshRate();

		void Render();
		static void Shutdown();
		static GUI& GetInstance();

		static void AddTextToConsole();
		static inline string addedText;
	private:
		//text filter for searching
		static inline ImGuiTextFilter textFilter;

		static inline bool filterTextProcessed;

		void RenderTopBar();

		//top bar interactions
		void TB_CheckVersion();
		void TB_ReportIssue();

		//rendered windows
		void RenderKeybindsMenu();
		void RenderDebugMenu();
		void RenderConsole();
		void RenderLightsMenu();

		//top bar rendered windows
		void RenderVersionCheckWindow();
		static inline string versionCompare;
		static inline string versionConfirm;

		//rendered debug window parts
		void RWPart_DebugMenuInfo();
		void RWPart_Interactions();
	};
}