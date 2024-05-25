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
	class GameGUI
	{
	public:
		static inline float fontScale;

		static inline ImVec2 initialPos;
		static inline ImVec2 initialSize;

		static inline ImVec2 minSize;
		static inline ImVec2 maxSize;

		static inline string targetScene;

		static void Initialize();

		static void CustomizeImGuiStyle();

		int GetScreenWidth();
		int GetScreenHeight();
		static float GetScreenRefreshRate();

		static void Render();

		static void FloatingWindow();

		static void Shutdown();
	};
}