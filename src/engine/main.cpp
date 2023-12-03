//external
#include "glad.h"
#include "magic_enum.hpp"

//engine
#include "render.h"
#include "shader.h"
#include "input.h"
#include "console.h"
#include "shutdown.h"

#include <iostream>
#include <string>
#include <sstream>

int main()
{
	ConsoleManager::WriteConsoleMessage(
		ConsoleManager::Caller::ENGINE, 
		ConsoleManager::Type::INFO, 
		"Initializing Elypso engine...\n");

	InputManager::InputSetup();

	if (RenderManager::WindowSetup() != 0) {
		ShutdownManager::Shutdown();
		return -1;
	}

	ShaderManager::ShaderSetup();

	if (!ShaderManager::shaderSetupSuccess)
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::Caller::SHADER, 
			ConsoleManager::Type::ERROR, 
			"Shader setup was unsuccessful!");
		return -1;
	}
	else
	{
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
	}
}