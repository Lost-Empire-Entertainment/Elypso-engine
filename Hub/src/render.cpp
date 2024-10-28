//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>
#include <filesystem>

#include "glad.h"
#include "stb_image.h"

#include "render.hpp"
#include "core.hpp"
#include "gui.hpp"

using std::cout;
using std::string;
using std::filesystem::current_path;

using Core::Hub;
using Graphics::GUI::GUI_Hub;

namespace Graphics
{
	void Render::Initialize()
	{
		cout << "Initializing GLFW...\n";

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		cout << "Initializing window...\n";

		if (currentWidth == 0) currentWidth = 1280;
		if (currentHeight == 0) currentHeight = 720;
		window = glfwCreateWindow(
			currentWidth,
			currentHeight,
			"Elypso hub 1.0.0",
			NULL,
			NULL);

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, Render::UpdateAfterRescale);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
		glfwSetWindowSizeLimits(window, 600, 300, 7680, 4320);
		glfwSwapInterval(1);

		Hub::defaultPath = current_path().generic_string();

		int width, height, channels;
		string iconpath = Hub::defaultPath.string() + "/icon.png";
		unsigned char* iconData = stbi_load(iconpath.c_str(), &width, &height, &channels, 4);

		GLFWimage icon{};
		icon.width = width;
		icon.height = height;
		icon.pixels = iconData;

		glfwSetWindowIcon(window, 1, &icon);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		cout << "Initializing GLAD...\n";

		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		UpdateAfterRescale(window, currentWidth, currentHeight);

		GUI_Hub::Initialize();
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Calculate the new aspect ratio
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		//Set the viewport based on the aspect ratio
		glViewport(0, 0, width, height);

		currentWidth = width;
		currentHeight = height;
	}

	void Render::Run()
	{

	}
}