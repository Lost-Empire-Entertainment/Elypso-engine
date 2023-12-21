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
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "type_ptr.hpp"

//engine
#include "console.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "searchUtils.hpp"
#include "shutdown.hpp"
#include "texture.hpp"
#include "timeManager.hpp"
#include "searchUtils.hpp"

#include <string>
#include <iostream>
#include <filesystem>

using glm::perspective;
using glm::radians;
using glm::translate;
using std::filesystem::exists;
using std::filesystem::current_path;
using std::cout;
using std::endl;

using Core::Input;
using Core::TimeManager;
using Core::ConsoleManager;
using Core::Engine;
using Utils::Search;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics
{
	Shader* Render::lightShader;
	Shader* Render::lightCubeShader;
	Input Render::camera(Render::window);
	vec3 lightPos(1.2f, 1.0f, 2.0f);

	void Render::RenderSetup()
	{
		Render::GLFWSetup();
		Render::WindowSetup();
		Render::GladSetup();

		Render::ContentSetup();

		GUI::GetInstance().Initialize();

		Input::InputSetup();

		TimeManager::targetDT = 1.0f / Graphics::GUI::GetScreenRefreshRate();
		TimeManager::lastTime = high_resolution_clock::now();
	}

	void Render::GLFWSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::GLFW,
			Type::DEBUG,
			"Initializing GLFW...\n");

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		ConsoleManager::WriteConsoleMessage(
			Caller::GLFW,
			Type::DEBUG,
			"GLFW initialized successfully!\n\n");
	}
	void Render::WindowSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_SETUP,
			Type::DEBUG,
			"Creating window...\n");

		//create a window object holding all the windowing data
		window = glfwCreateWindow(
			SCR_WIDTH,
			SCR_HEIGHT,
			("Elypso engine " + Engine::version).c_str(),
			NULL,
			NULL);

		if (window == NULL)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::GLFW,
				Type::EXCEPTION,
				"Error: Failed to create GLFW window!\n\n");
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
		glfwSetWindowSizeLimits(window, 1280, 720, 7680, 4320);
		glfwSwapInterval(1);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
		glfwSetScrollCallback(window, Input::ScrollCallback);
		glfwSetKeyCallback(window, Input::KeyCallback);

		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_SETUP,
			Type::DEBUG,
			"Window initialized successfully!\n\n");
	}
	void Render::GladSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::GLAD,
			Type::DEBUG,
			"Initializing GLAD...\n");

		//check if glad is initialized before continuing
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::GLAD,
				Type::EXCEPTION,
				"Error: Failed to initialize GLAD!\n\n");
			return;
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::GLAD,
			Type::DEBUG,
			"GLAD initialized successfully!\n\n");
	}
	void Render::ContentSetup()
	{
		glEnable(GL_DEPTH_TEST);

		string currentPath = Search::FindCurrentPath();
		lightShader = new Shader(
			currentPath + "/files/shaders/lightShader.vert", 
			currentPath + "/files/shaders/lightShader.frag");
		lightCubeShader = new Shader(
			currentPath + "/files/shaders/lightCubeShader.vert", 
			currentPath + "/files/shaders/lightCubeShader.frag");

		float vertices[] = 
		{
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};

		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glGenVertexArrays(1, &lightCubeVAO);
		glBindVertexArray(lightCubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		UpdateAfterRescale(window, SCR_WIDTH, SCR_HEIGHT);
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Calculate the new aspect ratio
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		//Set the viewport based on the aspect ratio
		glViewport(0, 0, width, height);
	}

	void Render::Shutdown()
	{
		delete lightShader;
		delete lightCubeShader;
		lightShader = nullptr;
		lightCubeShader = nullptr;
	}

	void Render::WindowLoop()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightShader->Use();
		lightShader->SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightShader->SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

		//camera transformation
		Input::ProcessInput(Render::window);
		mat4 projection = perspective(
			radians(fov),
			aspectRatio,
			nearClip,
			farClip);
		mat4 view = camera.GetViewMatrix() * lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		lightShader->SetMat4("projection", projection);
		lightShader->SetMat4("view", view);

		//world transformation
		mat4 model = mat4(1.0f);
		lightShader->SetMat4("model", model);

		//render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//render the lamp object
		lightCubeShader->Use();
		lightCubeShader->SetMat4("projection", projection);
		lightCubeShader->SetMat4("view", view);
		model = mat4(1.0f);
		model = translate(model, lightPos);
		model = scale(model, vec3(0.2f));
		lightCubeShader->SetMat4("model", model);

		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		GUI::GetInstance().Render();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}