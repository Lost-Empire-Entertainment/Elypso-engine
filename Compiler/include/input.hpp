//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "glfw3.h"

namespace Core
{
	class Input
	{
	public:
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void MouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
	};
}