//external
#include "glad.h"

//engine
#include "input.h"
#include "console.h"
#include "render.h"

namespace Core::Input
{
	void InputManager::InputSetup()
	{
		key[Key::Escape] = GLFW_KEY_ESCAPE;
		key[Key::Enter] = GLFW_KEY_ENTER;
	}

	void InputManager::ProcessInput(GLFWwindow* window)
	{
		if (glfwGetKey(Core::Graphics::Render::window, static_cast<int>(key[Key::Escape])) == GLFW_PRESS)
		{
			Core::Console::ConsoleManager::WriteConsoleMessage(
				Core::Console::ConsoleManager::Caller::INPUT,
				Core::Console::ConsoleManager::Type::SUCCESS,
				"User pressed ESC key to shut down engine...\n\n");

			glfwSetWindowShouldClose(window, true);
		}
	}
}