//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if GAME_MODE
#else
namespace Graphics::GUI
{
	class GameGUI
	{
	public:
		static inline bool isImguiInitialized;

		static void Initialize();
		static void Render();
		static void Shutdown();
	};
}
#endif