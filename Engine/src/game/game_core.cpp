//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

//external
#include "glfw3.h"

//engine
#include "core.hpp"
#include "console.hpp"
#include "render.hpp"
#include "timeManager.hpp"
#include "physics.hpp"

//game
#include "game_core.hpp"

using std::cout;

using Core::Engine;
using Core::ConsoleManager;
using Graphics::Render;
using Core::TimeManager;
using Core::Physics;

namespace Game
{
	void Game_Core::Initialize()
	{
		Engine::InitializeEngine();

		Engine::isRunning = true;
		if (!ConsoleManager::storedLogs.empty())
		{
			ConsoleManager::PrintLogsToBuffer();
		}
	}

	void Game_Core::Run()
	{
		if (Engine::isRunning)
		{
			TimeManager::UpdateDeltaTime();

			Physics::Update(TimeManager::deltaTime);

			Render::WindowLoop();
		}
	}
	bool Game_Core::ShouldClose()
	{
		if (glfwWindowShouldClose(Render::window))
		{
			Engine::isRunning = false;
			return true;
		}
		return false;
	}

	void Game_Core::Shutdown()
	{
		Engine::Shutdown(false, true);
	}
}
