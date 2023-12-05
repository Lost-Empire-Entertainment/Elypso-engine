//engine
#include "render.h"
#include "shader.h"
#include "input.h"
#include "console.h"
#include "shutdown.h"
#include "core.h"

int main()
{
	Core::ConsoleManager::WriteConsoleMessage(
		Core::ConsoleManager::Caller::ENGINE,
		Core::ConsoleManager::Type::INFO,
		"Initializing " + Core::name + " " + Core::version + "...\n");

	Core::InputManager::InputSetup();

	if (Graphics::RenderManager::WindowSetup() != 0) 
	{
		Core::ShutdownManager::Shutdown();
		std::cin.get();
		return -1;
	}

	Graphics::ShaderManager::ShaderSetup();

	Core::ConsoleManager::WriteConsoleMessage(
		Core::ConsoleManager::Caller::WINDOW_LOOP,
		Core::ConsoleManager::Type::INFO,
		"Entering window loop...\n");

	while (!glfwWindowShouldClose(Graphics::RenderManager::window))
	{
		Core::InputManager::ProcessInput(Graphics::RenderManager::window);

		Graphics::RenderManager::WindowLoop();
	}

	Core::ConsoleManager::WriteConsoleMessage(
		Core::ConsoleManager::Caller::WINDOW_LOOP,
		Core::ConsoleManager::Type::INFO,
		"Exiting window loop...\n");

	Core::ShutdownManager::Shutdown();
	std::cin.get();
	return 0;
}