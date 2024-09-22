#include "core.hpp"
#include "gameLoop.hpp"

using Core::Game;

namespace Core
{
	void GameTest::CoreTest()
	{
		Game::Initialize();
		Game::Run();
	}
}