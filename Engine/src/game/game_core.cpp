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
#include "input.hpp"

//game
#include "game_core.hpp"

using std::cout;

using Core::Engine;
using Graphics::Render;
using Core::TimeManager;
using Core::Physics;
using Core::Input;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

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

	void Game_Core::SetRightClickState(bool rightClickState)
	{
		Input::rightClickState = rightClickState;
		string state = rightClickState ? "true" : "false";

		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::DEBUG,
			"Set right mouse state to " + state + "!\n");
	}

	void Game_Core::SetYMovementState(bool yMovementState)
	{
		Input::allowYMovement = yMovementState;
		string state = yMovementState ? "true" : "false";

		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::DEBUG,
			"Set y movement state to " + state + "!\n");
	}

	void Game_Core::SetRotationState(bool rotationState)
	{
		Input::allowRotation = rotationState;
		string state = rotationState ? "true" : "false";

		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::DEBUG,
			"Set rotation state to " + state + "!\n");
	}

	void Game_Core::SetMovementState(bool movementState)
	{
		Input::allowMovement = movementState;
		string state = movementState ? "true" : "false";

		ConsoleManager::WriteConsoleMessage(
			Caller::INPUT,
			Type::DEBUG,
			"Set movement state to " + state + "!\n");
	}
}
