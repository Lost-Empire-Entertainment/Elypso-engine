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
		static inline unsigned int VBO, VAO, EBO;

		static inline GLFWwindow* window;

		static int WindowSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void Shutdown();
		static void WindowLoop();
	private:
		static Shader* shader;

		static bool shaderInitialized;

		static inline const unsigned int SCR_WIDTH = 1200;
		static inline const unsigned int SCR_HEIGHT = 1200;

		static inline glm::vec3 cubePositions[10];
	};
}

#endif