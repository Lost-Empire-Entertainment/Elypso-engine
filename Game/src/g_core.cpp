//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "magic_enum.hpp"
#include "glfw3.h"
#include "glm.hpp"

//engine
#include "game_core.hpp"

//game
#include "g_core.hpp"
#include "g_ui.hpp"
#include "g_input.hpp"
#include "g_player.hpp"
#include "g_console.hpp"

using Game::Game_Core;

namespace GameTemplate
{
	void G_Core::Initialize()
	{
		Game_Core::Initialize();

		G_UI::AddWindowsToList();
		G_Input::Initialize();
		G_Player::Initialize();

		G_Console::AddCommands();
	}

	void G_Core::Run()
	{
		while (!Game_Core::ShouldClose())
		{
			Game_Core::Run();

			G_Input::UpdateKeyStates();
			G_Input::UpdateMouse();

			G_Player::UpdatePlayerPos();
		}
	}

	void G_Core::Shutdown()
	{
		Game_Core::Shutdown();
	}
}
