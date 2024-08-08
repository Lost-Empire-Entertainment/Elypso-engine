//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//external
#include "glfw3.h"

//game
#include "camera.hpp"

using Graphics::Camera;

namespace Graphics
{
	class Render
	{
	public:
		static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

		static inline mat4 projection;
		static inline mat4 view;

		static inline GLFWwindow* window;
		static Camera camera;

		static void Initialize();
		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();

		static void ContentSetup();

		static void WindowLoop();

		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
	};
}