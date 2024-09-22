//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
namespace Graphics::GUI
{
	class GUILinks
	{
	public:
		static inline bool renderLinksWindow;

		static void RenderLinksWindow();
	private:
		static void RenderLinksContent();
	};
}
#endif