#pragma once

#ifndef GRAPHICS_RENDER_H
#define GRAPHICS_RENDER_H

//external
#include "glad.h"
#include "glfw3.h"

//engine
#include "shader.h"

#include <string>

using namespace glm;

namespace Graphics
{
	class Render
	{
	public:
		static inline const unsigned int SCR_WIDTH = 1200;
		static inline const unsigned int SCR_HEIGHT = 1200;

		static inline unsigned int VBO, VAO, EBO;

		static inline float cameraSpeed;
		static inline vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
		static inline vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
		static inline vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

		static inline GLFWwindow* window;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void Shutdown();
		static void WindowLoop();
	private:

		static Shader* shader;

		static inline vec3 cubePositions[10];

		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void ContentSetup();
	};
}

#endif