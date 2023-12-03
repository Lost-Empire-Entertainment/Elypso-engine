//external
#include "glad.h"

//engine
#include "input.h"
#include "console.h"
#include "render.h"

void InputManager::InputSetup() 
{
	key[Key::Escape] = GLFW_KEY_ESCAPE;
	key[Key::Enter] = GLFW_KEY_ENTER;
}

void InputManager::ProcessInput(GLFWwindow* window) 
{
	if (glfwGetKey(RenderManager::window, static_cast<int>(key[Key::Escape])) == GLFW_PRESS)
	{
		ConsoleManager::WriteConsoleMessage(
			ConsoleManager::Caller::INPUT,
			ConsoleManager::Type::SUCCESS,
			"User pressed ESC key to shut down engine...\n\n");

		glfwSetWindowShouldClose(window, true);
	}
}