//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

//engine
#include "core.hpp"

//game
#include "gameLoop.hpp"

using std::cout;

using Core::Engine;

namespace Core
{
	void Game::Initialize()
	{
		cout << "Game initialization...\n";

		Engine::InitializeEngine();
	}

	void Game::Run()
	{
		cout << "Game run loop...\n";
	}
}