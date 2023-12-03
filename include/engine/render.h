#pragma once

//external
#include "glfw3.h"

//engine
#include "console.h"

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