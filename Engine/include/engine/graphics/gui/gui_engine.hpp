//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <vector>
#include <string>

//external
#include "imgui.h"

namespace Graphics::GUI
{
	using std::vector;
	using std::string;

	class EngineGUI
	{
	public:
		static inline bool renderUnsavedShutdownWindow;
		static inline bool renderUnsavedSceneSwitchWindow;

		static inline string targetScene;

		static void Initialize();

		/// <summary>
		/// Takes in ImGui window height and width and returns position of window so it will be centered according to current GLFW window size.
		/// </summary>
		static ImVec2 CenterWindow(const ImVec2& size);

		static void Render();
		static void Shutdown();
	private:
		static inline bool isImguiInitialized;

		enum class SaveBeforeState
		{
			sceneSwitch,
			shutdown
		};

		//text filter for searching
		static ImGuiTextFilter textFilter;

		static inline bool filterTextProcessed;

		static void SetStyle();

		static void RenderTopBar();

		static void SaveBefore(SaveBeforeState saveBeforeState);
	};
}
#endif