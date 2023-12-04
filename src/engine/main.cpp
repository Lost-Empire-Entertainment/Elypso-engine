//engine
#include "render.h"
#include "shader.h"
#include "input.h"
#include "console.h"
#include "shutdown.h"

int main()
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::ENGINE, 
		ConsoleManager::Type::INFO, 
		"Initializing Elypso engine...\n");

	InputManager::InputSetup();

	if (RenderManager::WindowSetup() != 0) 
	{
		ShutdownManager::Shutdown();
		std::cin.get();
		return -1;
	}

	ShaderManager::ShaderSetup();

	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::WINDOW_LOOP,
		ConsoleManager::Type::INFO,
		"Entering window loop...\n");

	while (!glfwWindowShouldClose(RenderManager::window))
	{
		InputManager::ProcessInput(RenderManager::window);

		RenderManager::WindowLoop();
	}

	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::WINDOW_LOOP,
		ConsoleManager::Type::INFO,
		"Exiting window loop...\n");

	ShutdownManager::Shutdown();
	std::cin.get();
	return 0;
}