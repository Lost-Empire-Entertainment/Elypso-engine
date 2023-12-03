//external
#include "glad.h"
#include "glfw3.h"
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
		ConsoleManager::MessageType::ENGINE, 
		ConsoleManager::ErrorType::INFO, 
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
			ConsoleManager::MessageType::SHADER, 
			ConsoleManager::ErrorType::ERROR, 
			"Shader setup was unsuccessful!");
		return -1;
	}
	else
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::WINDOW_LOOP,
			ConsoleManager::ErrorType::INFO,
			"Entering window loop...\n");

		while (!glfwWindowShouldClose(RenderManager::window)) RenderManager::WindowLoop();

		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::MessageType::WINDOW_LOOP,
			ConsoleManager::ErrorType::INFO,
			"Exiting window loop...\n");

		ShutdownManager::Shutdown();
	}
}