//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//game
#include "core.hpp"

using Core::Game;

int main()
{
	Game::Initialize();

	Game::Run();

	Game::Shutdown();
	return 0;
}