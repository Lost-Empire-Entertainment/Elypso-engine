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
	const std::string version = "0.0.3 prototype";

	int InitializeEngine()
	{
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::ENGINE,
			Core::Console::ConsoleManager::Type::INFO,
			"Initializing " + Core::name + " " + Core::version + "...\n");

		Core::Input::InputManager::InputSetup();

		if (Graphics::RenderManager::WindowSetup() != 0)
		{
			Core::Console::ConsoleManager::WriteConsoleMessage(
				Core::Console::ConsoleManager::Caller::WINDOW_SETUP,
				Core::Console::ConsoleManager::Type::ERROR,
				"Window setup was unsuccessful!\n\n");

			Core::ShutdownManager::Shutdown();
			std::cin.get();
			return -1;
		}

		Graphics::ShaderManager::ShaderSetup();

		if (!Graphics::ShaderManager::shaderSetupSuccess)
		{
			Core::Console::ConsoleManager::WriteConsoleMessage(
				Core::Console::ConsoleManager::Caller::SHADER,
				Core::Console::ConsoleManager::Type::ERROR,
				"Shader setup was unsuccessful!\n\n");

			Core::ShutdownManager::Shutdown();
			std::cin.get();
			return -1;
		}
		else return 0;
	}

	void RunEngine()
	{
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::WINDOW_LOOP,
			Core::Console::ConsoleManager::Type::INFO,
			"Entering window loop...\n");

		while (!glfwWindowShouldClose(Graphics::RenderManager::window))
		{
			Core::Input::InputManager::ProcessInput(Graphics::RenderManager::window);

			Graphics::RenderManager::WindowLoop();
		}

		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::WINDOW_LOOP,
			Core::Console::ConsoleManager::Type::INFO,
			"Exiting window loop...\n");
	}

	int ShutdownEngine()
	{
		Core::ShutdownManager::Shutdown();
		std::cin.get();
		return 0;
	}
}