#include <string>

#include "gameLoop.hpp"
#include "core.hpp"

using Core::Game;

using std::string;

namespace Core
{
	void GameTest::CoreTest()
	{
		string val1 = "Game template";
		string val2 = "v0.1";
		Game::Initialize(val1, val2);

		Game::Run();
	}
}