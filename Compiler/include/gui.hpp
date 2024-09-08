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
		static inline bool isBuilding;
		static inline string targetFolder;
		static inline string compilableProgramName;
		static inline vector<string> output;

		static void GUIInitialize();
		static void GUILoop();

		static int GetScreenHeight();
		static int GetScreenWidth();

		static void RenderParentWindow();
		static void RenderWindowContent();

		static void GUIShutdown();
	private:
		static inline bool hasBuiltOnce;
		static inline bool firstScrollToBottom;
	};
}