//external
#include "glad.h"

//engine
#include "core.h"
#include "deltaTime.h"
#include "console.h"
#include "input.h"
#include "render.h"
#include "shutdown.h"

#include <string>

using namespace Graphics;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void InitializeEngine()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Initializing " + name + " " + version + "...\n");

		InputManager::InputSetup();

		Render::RenderSetup();
	}

	void RunEngine()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_LOOP,
			Type::INFO,
			"Entering window loop...\n");

		while (!glfwWindowShouldClose(Render::window))
		{
			DeltaTime::UpdateDeltaTime();

			InputManager::ProcessInput(Render::window);

			Render::WindowLoop();
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_LOOP,
			Type::INFO,
			"Exiting window loop...\n");
	}
}