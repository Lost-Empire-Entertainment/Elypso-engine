//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Graphics::GUI
{
	class GUICredits
	{
	public:
		static inline bool renderCreditsWindow;

		static void RenderCreditsWindow();
	private:
		static void RenderCreditsContent();
	};
}