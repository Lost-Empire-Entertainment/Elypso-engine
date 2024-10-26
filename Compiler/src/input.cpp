//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "input.hpp"
#include "core.hpp"

namespace Core
{
	void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Compiler::UpdateActivityTime();
	}

	void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		Compiler::UpdateActivityTime();
	}

	void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Compiler::UpdateActivityTime();
	}

	void Input::MouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
	{
		Compiler::UpdateActivityTime();
	}
}