//external
#include "glad.h"

//engine
#include "core.h"
#include "console.h"
#include "input.h"
#include "render.h"
#include "shutdown.h"

#include <string>

/*
* =========================================
*
* The core script of the engine, responsible for both
* InitializeEngine and RunEngine methods that are
* run in main.cpp to keep that script clean.
*
* =========================================
*/

namespace Core
{
	const std::string name = "Elypso engine";
	const std::string version = "0.0.4 prototype";

	void InitializeEngine()
	{
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::ENGINE,
			Core::Console::ConsoleManager::Type::INFO,
			"Initializing " + Core::name + " " + Core::version + "...\n");

		Core::Input::InputManager::InputSetup();

		Core::Graphics::Render::RenderSetup();
	}

	void RunEngine()
	{
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::WINDOW_LOOP,
			Core::Console::ConsoleManager::Type::INFO,
			"Entering window loop...\n");

		while (!glfwWindowShouldClose(Core::Graphics::Render::window))
		{
			Core::Input::InputManager::ProcessInput(Core::Graphics::Render::window);

			Core::Graphics::Render::WindowLoop();
		}

		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::WINDOW_LOOP,
			Core::Console::ConsoleManager::Type::INFO,
			"Exiting window loop...\n");
	}
}