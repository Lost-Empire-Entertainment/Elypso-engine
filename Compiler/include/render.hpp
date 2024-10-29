//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "glfw3.h"
#include "glm.hpp"

namespace Graphics
{
	using glm::vec3;

	class Render
	{
	public:
		static inline GLFWwindow* window;
		static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

		static void RenderInitialize();
		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();

		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);

		static void RenderLoop();
	};
}