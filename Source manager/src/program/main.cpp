//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//external
#include "glad.h"
#include "glfw3.h"

//program
#include "main.hpp"

using std::cout;
using std::endl;

using Core::Render;

int main()
{
	Render::GLFWSetup();
	Render::WindowSetup();
	Render::GladSetup();
	Render::GUISetup();

	Render::WindowLoop();

	Render::Shutdown();

	return 0;
}

namespace Core
{
	void Render::GLFWSetup()
	{
		cout << "Initializing GLFW..." << endl;

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		cout << "GLFW initialized successfully!" << endl;
	}

	void Render::WindowSetup()
	{
		cout << "Creating window..." << endl;

		window = glfwCreateWindow(
			width,
			height,
			"Source manager",
			NULL,
			NULL);

		if (window == NULL)
		{
			cout << "Error: Failed to created GLFW window!" << endl;
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetWindowSizeLimits(window, 1280, 720, 1280, 720);
		glfwSwapInterval(1);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		//glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
		//glfwSetScrollCallback(window, Input::ScrollCallback);
		//glfwSetKeyCallback(window, Input::KeyCallback);

		cout << "Window initialized successfully!" << endl;
	}

	void Render::GladSetup()
	{
		cout << "Initializing glad..." << endl;

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << "Error: Failed to initialize GLAD!" << endl;
			return;
		}

		cout << "GLAD initialized successfully!" << endl;
	}

	void Render::GUISetup()
	{

	}

	void Render::WindowLoop()
	{
		cout << "Entering window loop..." << endl;

		while (!glfwWindowShouldClose(Render::window))
		{
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	void Render::Shutdown()
	{

	}
}