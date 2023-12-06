#pragma once

#ifndef GRAPHICS_RENDER_H
#define GRAPHICS_RENDER_H

//external
#include "glad.h"
#include "glfw3.h"

//engine
#include "shader.h"

#include <string>

namespace Graphics
{
	class Render
	{
	public:
		static bool shaderInitialized;

		static inline const unsigned int SCR_WIDTH = 800;
		static inline const unsigned int SCR_HEIGHT = 600;

		static inline unsigned int VBO, VAO;

		static inline GLFWwindow* window;

		static Shader* shader;

		static int WindowSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void Shutdown();
		static void WindowLoop();
	};
}

#endif