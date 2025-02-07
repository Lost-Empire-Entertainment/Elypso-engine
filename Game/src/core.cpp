//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>

//game
#include "gameLoop.hpp"
#include "core.hpp"
#include "gui.hpp"

//engine
using Core::Game;

//game
using TheGameGUI::GUI;

using std::string;

namespace GameCore
{
	void GameTemplate::InitializeGame()
	{
		Game::Initialize();
		GUI::AddWindowsToList();
	}

	void GameTemplate::RunGame()
	{
		Game::Run();
	}
}
