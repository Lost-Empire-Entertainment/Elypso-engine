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

#include <vector>
#include <filesystem>

using std::vector;
using std::filesystem::path;

namespace Graphics::GUI
{
	class EngineGUI
	{
	public:
		static inline bool writeToConsole;
		static inline bool firstScrollToBottom;
		static inline bool allowScrollToBottom;

		static inline bool showDebugMenu;
		static inline bool showConsole;
		static inline bool showSceneMenu;
		static inline bool showProjectHierarchyWindow;
		static inline bool showVersionWindow;
		static inline bool outdatedVersion;

		static inline float fontScale;

		static inline char inputTextBuffer[128];
		static inline vector<string> consoleMessages;
		static inline const int maxConsoleMessages = 1000;

		void Initialize();

		void CustomizeImGuiStyle();

		int GetScreenWidth();
		int GetScreenHeight();
		static float GetScreenRefreshRate();

		void Render();
		static void Shutdown();
		static EngineGUI& GetInstance();

		static void AddTextToConsole(const string& message);
	private:
		//text filter for searching
		static inline ImGuiTextFilter textFilter;

		static inline bool filterTextProcessed;

		void RenderTopBar();

		//top bar interactions
		void TB_CheckVersion();
		void TB_ReportIssue();

		//top bar rendered windows
		void RenderVersionCheckWindow();
		static inline string versionCompare;
		static inline string versionConfirm;

		//rendered windows
		void RenderDebugMenu();
		void RenderConsole();
		void RenderSceneMenu();
		void RenderProjectHierarchyWindow();

		//rendered debug window parts
		void RD_DebugMenuInfo();
		void RD_Interactions();

		//rendered scene menu parts
		void RSM_Main();
		void RSM_Spot();
		void RSM_Point();
		void RSM_Cube();

		//rendered project hierarchy window parts
		void RPHW_FoldersAndFiles(path selectedPath);
	};
}