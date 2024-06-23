//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>
#include <string>
#include <vector>

//external
#include "glfw3.h"
#include "glm.hpp"
#include "imgui.h"

using std::string;
using std::filesystem::path;
using glm::vec3;
using std::vector;

class Core
{
public:
	static inline GLFWwindow* window;

	static inline unsigned int currentWidth;
	static inline unsigned int currentHeight;

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

	static void LoadConfigFile();
	static void SaveConfigFile();
	static vector<string> StringSplit(const string& input, char delimiter);

	static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
	static void Render();

	static void CreateErrorPopup(const char* errorTitle = "INSERT ERROR TITLE", const char* errorMessage = "INSERT ERROR MESSAGE");

	static bool IsThisProcessAlreadyRunning(const string& processName);

	static void Shutdown(bool immediate = false);
};