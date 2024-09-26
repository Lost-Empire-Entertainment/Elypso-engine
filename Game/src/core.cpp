//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>

//game
#include "gameLoop.hpp"
#include "core.hpp"
#include "gui.hpp"

using Core::Game;
using TheGameGUI::GUICore;

using std::string;

namespace Core
{
	void GameCore::InitializeGame()
	{
		string version = "0.1";
		Game::Initialize(version);
		GUICore::AddWindowsToList();
	}

	void GameCore::RunGame()
	{
		Game::Run();
	}
}