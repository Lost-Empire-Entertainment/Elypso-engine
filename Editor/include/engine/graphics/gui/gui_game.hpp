//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#else

#include <vector>
#include <functional>

//external
#include "imgui.h"

namespace Graphics::GUI
{
	using std::vector;
	using std::function;

	class GameGUI
	{
	public:
		static inline bool isImguiInitialized;
		static inline vector<function<void()>> imguiWindows;

		static void Initialize();
		static void Render();

		/// <summary>
		/// Takes in ImGui window height and width and returns position of window so it will be centered according to current GLFW window size.
		/// </summary>
		static ImVec2 CenterWindow(const ImVec2& size);

		static void Shutdown();
	private:
		static void SetStyle();
	};
}
#endif