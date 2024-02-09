#pragma once

//external
#include "glfw3.h"
#include "glm.hpp"
#include "imgui.h"

#include <filesystem>
#include <vector>
#include <string>

using std::string;
using std::vector;
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
	static inline path projectsPath;

	static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

	static void Initialize();
	static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
	static void Render();
	static void Shutdown();
};

class GUI
{
public:
	static inline int framebufferWidth, framebufferHeight;
	static inline float panelSpacing = 10.0f;
	static inline float panelHeight = 200.0f;

	static void Initialize();

	static void Render();
	static void RenderPanels(const vector<string>& files);
	static void RenderButtons();

	static vector<string> GetFiles(const string& path);

	static void NewProject();

	static void AddProject();

	static void Shutdown();
};