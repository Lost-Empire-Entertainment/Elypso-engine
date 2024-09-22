//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Graphics
{
	class GUI
	{
	public:
		enum class Target
		{
			Hub,
			Engine,
			EngineLib
		};
		static inline Target target;

		static inline vector<string> output;

		static void GUIInitialize();
		static void GUILoop();

		static int GetScreenHeight();
		static int GetScreenWidth();

		static void RenderParentWindow();
		static void RenderWindowContent();
		static void FinishCompile();

		static void GUIShutdown();
	private:
		static inline bool isImguiInitialized;

		enum class Action
		{
			compile,
			clean_rebuild
		};
		static inline Action action;

		static inline bool isBuilding;
		static inline bool hasBuiltOnce;
		static inline bool firstScrollToBottom;
		static inline bool sentMsg;
	};
}