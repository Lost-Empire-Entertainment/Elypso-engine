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
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

#pragma once

//external
#include "imgui.h"

namespace Graphics
{
	class GUI
	{
	public:
		static inline bool writeToConsole;
		static inline bool allowScrollToBottom = true;

		static inline float fontScale = 1.5f;

		//holds text content
		static inline ImGuiTextBuffer textBuffer;

		void Initialize();

		void CustomizeImGuiStyle();

		int GetScreenWidth();
		int GetScreenHeight();
		static float GetScreenRefreshRate();

		void Render();
		void Shutdown();
		static GUI& GetInstance();

		static void AddTextToConsole();
		static inline string addedText;
	private:
		//text filter for searching
		static inline ImGuiTextFilter textFilter;

		static inline bool filterTextProcessed;

		//rendered ui
		void RenderDebugMenu();
		void RenderSlider();
		void RenderConsole();

		//debug groups
		void RDM_Info();
		void RDM_GeneralKeys();
		void RDM_DebugButtons();

		//slider groups
		void RS_CameraClipRange();
		void RS_MoveSpeedMultiplier();
		void RS_FOV();
	};
}