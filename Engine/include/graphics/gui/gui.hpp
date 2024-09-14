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
		static inline bool renderUnsavedShutdownWindow;
		static inline bool renderUnsavedSceneSwitchWindow;

		//debug windows
		static inline bool renderAboutWindow;
		static inline bool renderDebugLogWindow;
		static inline bool renderDemoWindow;
		static inline bool renderIDStackWindow;
		static inline bool renderMetricsWindow;
		static inline bool renderStackToolWindow;
		static inline bool renderStyleEditorWindow;
		static inline bool renderUserGuideWindow;

		static inline string targetScene;

		static void Initialize();

		/// <summary>
		/// Takes in ImGui window height and width and returns position of window so it will be centered according to current GLFW window size.
		/// </summary>
		static ImVec2 CenterWindow(const ImVec2& size);

		static void Render();
		static void Shutdown();
	private:
		enum class SaveBeforeState
		{
			sceneSwitch,
			shutdown
		};

		//text filter for searching
		static ImGuiTextFilter textFilter;

		static inline bool filterTextProcessed;

		static void RenderTopBar();

		static void SaveBefore(SaveBeforeState saveBeforeState);
	};
}