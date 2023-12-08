//external
#include "glad.h"

//engine
#include "core.h"
#include "console.h"
#include "input.h"
#include "render.h"
#include "shutdown.h"

#include <string>

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

		Graphics::Render::RenderSetup();
	}

	void RunEngine()
	{
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::WINDOW_LOOP,
			Core::Console::ConsoleManager::Type::INFO,
			"Entering window loop...\n");

		while (!glfwWindowShouldClose(Graphics::Render::window))
		{
			Core::Input::InputManager::ProcessInput(Graphics::Render::window);

			Graphics::Render::WindowLoop();
		}

		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::WINDOW_LOOP,
			Core::Console::ConsoleManager::Type::INFO,
			"Exiting window loop...\n");
	}
}