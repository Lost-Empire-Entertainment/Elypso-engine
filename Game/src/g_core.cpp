//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "game_core.hpp"

//game
#include "g_core.hpp"
#include "g_ui.hpp"

using Game::Game_Core;

namespace GameTemplate
{
	void G_Core::Initialize()
	{
		Game_Core::Initialize();
		G_UI::AddWindowsToList();
	}

	void G_Core::Run()
	{
		while (!Game_Core::ShouldClose())
		{
			Game_Core::Run();
		}
	}

	void G_Core::Shutdown()
	{
		Game_Core::Shutdown();
	}
}
