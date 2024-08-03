//This script is a part of the Game Template that is distributed by Elypso Engine.
//If you have access to this script then you have full freedom to do anything with it.
//A copy of the MIT license has been included in LICENSE.md

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