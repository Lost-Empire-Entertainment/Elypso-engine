#include <string>

#include "gameLoop.hpp"
#include "core.hpp"

using Core::Game;

using std::string;

namespace Core
{
	void GameTest::CoreTest()
	{
		string val1 = "Game";
		string val2 = "0.1.1";
		Game::Initialize(val1, val2);

		Game::Run();
	}
}