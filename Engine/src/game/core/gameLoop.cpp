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
	void Game::Initialize(const string& version)
	{
		cout << "Game is initializing...\n";

		Engine::InitializeEngine(version);
	}

	void Game::Run()
	{
		cout << "Game entered runtime loop...\n";

		Engine::RunEngine();
	}
}