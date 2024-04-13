//Copyright(C) < 2024 > < Lost Empire Entertainment >

//game
#include "core.hpp"

using Core::Game;

int main()
{
	Game::InitializeGame();

	Game::RunGame();

	Game::Shutdown();
	return 0;
}