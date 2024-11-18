//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>
#include <filesystem>

#include "stb_image.h"
#include "glad.h"

#include "render.hpp"
#include "gui.hpp"
#include "core.hpp"
#include "input.hpp"

using std::cout;
using std::string;
using std::filesystem::path;

using Core::Compiler;
using Core::Input;

namespace Graphics
{
	void Render::RenderInitialize()
	{
		GLFWSetup();
		WindowSetup();
		GladSetup();

		GUI::GUIInitialize();
	}

	void Render::GLFWSetup()
	{
		cout << "Initializing GLFW...\n";

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		cout << "GLFW initialized successfully!\n\n";
	}

	void Render::WindowSetup()
	{
		cout << "Creating window...\n";

		//create a window object holding all the windowing data
		string name = "Compiler | Elypso hub";
		window = glfwCreateWindow(
			1000,
			1000,
			name.c_str(),
			NULL,
			NULL);

		if (window == NULL)
		{
			cout << "Failed to create GLFW window!\n\n";
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
		glfwSetWindowSizeLimits(window, 750, 750, 1500, 1500);
		glfwSwapInterval(1);

		int width, height, channels;
		string iconpath = (path(Compiler::filesPath) / "icon.png").string();
		cout << "icon path: " << iconpath << "\n";
		unsigned char* iconData = stbi_load(iconpath.c_str(), &width, &height, &channels, 4);

		GLFWimage icon{};
		icon.width = width;
		icon.height = height;
		icon.pixels = iconData;

		glfwSetWindowIcon(window, 1, &icon);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		glfwSetKeyCallback(window, Input::KeyCallback);
		glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
		glfwSetScrollCallback(window, Input::ScrollCallback);
		glfwSetCursorPosCallback(window, Input::MouseMovementCallback);

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { Compiler::MainShutdown(); });

		cout << "Window initialized successfully!\n\n";
	}

	void Render::GladSetup()
	{
		cout << "Initializing GLAD...\n";

		//check if glad is initialized before continuing
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << "Failed to initialize GLAD!\n\n";
			return;
		}

		cout << "GLAD initialized successfully!\n\n";
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Set the viewport based on the aspect ratio
		glViewport(0, 0, width, height);
	}

	void Render::RenderLoop()
	{
		glClearColor(
			backgroundColor.x,
			backgroundColor.y,
			backgroundColor.z,
			1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GUI::GUILoop();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		if (!Compiler::IsUserIdle())
		{
			glfwPollEvents();
		}
		else glfwWaitEvents();
	}
}