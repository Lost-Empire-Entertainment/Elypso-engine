//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Graphics::GUI
{
	class GUIDebugMenu
	{
	public:
		static inline bool renderDebugMenu;
		static void RenderDebugMenu();

		static void RD_DebugMenuInfo();
		static void RD_Interactions();
	};
}