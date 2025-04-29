//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "glfw3.h"

//engine
#include "render.hpp"

//game
#include "g_input.hpp"

using Graphics::Render;

namespace GameTemplate
{
	static GLFWwindow* window = nullptr;

	void G_Input::Initialize()
	{
		window = Render::window;

		keyStates["R"] = { GLFW_KEY_R }; //randomize point light color

		keyStates["Z"] = { GLFW_KEY_Z }; //toggle camera y movement lock
		keyStates["X"] = { GLFW_KEY_X }; //hold right mouse to rotate camera
		keyStates["C"] = { GLFW_KEY_C }; //toggle all camera movement lock
	}

	void G_Input::UpdateKeyStates()
	{
		for (auto& [name, state] : keyStates)
		{
			state.wasDown = state.isDown;
			state.isDown = glfwGetKey(window, state.key) == GLFW_PRESS;
		}
	}

	bool G_Input::IsPressed(const string& name)
	{
		auto it = keyStates.find(name);
		return it != keyStates.end()
			&& it->second.isDown
			&& !it->second.wasDown;
	}

	bool G_Input::IsHeld(const string& name)
	{
		auto it = keyStates.find(name);
		return it != keyStates.end()
			&& it->second.isDown;
	}
}