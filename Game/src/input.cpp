//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "glfw3.h"

//engine
#include "render.hpp"

//game
#include "input.hpp"

using Graphics::Render;

namespace GameTemplate
{
	static GLFWwindow* window = nullptr;

	void T_Input::Initialize()
	{
		window = Render::window;

		keyStates["R"] = { GLFW_KEY_R };
	}

	void T_Input::UpdateKeyStates()
	{
		for (auto& [name, state] : keyStates)
		{
			state.wasDown = state.isDown;
			state.isDown = glfwGetKey(window, state.key) == GLFW_PRESS;
		}
	}

	bool T_Input::IsPressed(const string& name)
	{
		auto it = keyStates.find(name);
		return it != keyStates.end()
			&& it->second.isDown
			&& !it->second.wasDown;
	}

	bool T_Input::IsHeld(const string& name)
	{
		auto it = keyStates.find(name);
		return it != keyStates.end()
			&& it->second.isDown;
	}
}