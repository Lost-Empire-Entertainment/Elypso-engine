#pragma once

//external
#include "glad.h"
#include "glfw3.h"

//engine
#include "shader.h"

#include <string>

namespace Graphics
{
	class RenderManager
	{
	public:
		static const unsigned int SCR_WIDTH = 800;
		static const unsigned int SCR_HEIGHT = 600;

		static inline GLFWwindow* window;

		static int WindowSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void WindowLoop();
	};
}