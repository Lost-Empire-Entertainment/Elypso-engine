//external
#include "glad.h"

//engine
#include "input.h"
#include "console.h"
#include "render.h"
#include "deltaTime.h"

using namespace glm;
using namespace Graphics;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	void InputManager::InputSetup()
	{
		key[Key::Escape] = GLFW_KEY_ESCAPE;
		key[Key::Enter] = GLFW_KEY_ENTER;
		key[Key::Forwards] = GLFW_KEY_W;
		key[Key::Backwards] = GLFW_KEY_S;
		key[Key::Left] = GLFW_KEY_A;
		key[Key::Right] = GLFW_KEY_D;
	}

	void InputManager::ProcessInput(GLFWwindow* window)
	{
		if (glfwGetKey(Render::window, static_cast<int>(key[Key::Escape])) == GLFW_PRESS)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INPUT,
				Type::SUCCESS,
				"User pressed ESC key to shut down engine...\n\n");

			glfwSetWindowShouldClose(window, true);
		}

		Render::cameraSpeed = static_cast<float>(2.5f * DeltaTime::deltatime);
		//camera forwards
		if (glfwGetKey(Render::window, static_cast<int>(key[Key::Forwards])) == GLFW_PRESS)
		{
			Render::cameraPos += 
				Render::cameraSpeed * Render::cameraFront;
		}
		//camera backwards
		if (glfwGetKey(Render::window, static_cast<int>(key[Key::Backwards])) == GLFW_PRESS)
		{
			Render::cameraPos -= 
				Render::cameraSpeed * Render::cameraFront;
		}
		//camera left
		if (glfwGetKey(Render::window, static_cast<int>(key[Key::Left])) == GLFW_PRESS)
		{
			Render::cameraPos -= 
				normalize(cross(Render::cameraFront, Render::cameraUp)) * Render::cameraSpeed;
		}
		//camera right
		if (glfwGetKey(Render::window, static_cast<int>(key[Key::Right])) == GLFW_PRESS)
		{
			Render::cameraPos +=
				normalize(cross(Render::cameraFront, Render::cameraUp)) * Render::cameraSpeed;
		}
	}
}