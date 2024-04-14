//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>
#include <string>

//external
#include "glfw3.h"
#include "glm.hpp"
#include "imgui.h"

using std::string;
using std::filesystem::path;
using glm::vec3;

class Core
{
public:
	static inline GLFWwindow* window;

	static inline unsigned int SCR_WIDTH = 1280;
	static inline unsigned int SCR_HEIGHT = 720;

	static inline int windowedWidth;
	static inline int windowedHeight;

	static inline float aspectRatio;

	static inline path defaultPath;
	static inline path docsPath;
	static inline path projectsFilePath;
	static inline path configFilePath;
	static inline path enginePath;

	static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

	static void Initialize();
	static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
	static void Render();
	static void Shutdown();
};