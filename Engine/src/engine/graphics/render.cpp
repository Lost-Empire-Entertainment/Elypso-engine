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
using glm::rotate;
using std::filesystem::exists;
using std::filesystem::current_path;
using std::cout;
using std::endl;
using std::to_string;

using Core::Input;
using Core::TimeManager;
using Core::ConsoleManager;
using Core::Engine;
using Utils::Search;
using Graphics::GUI::EngineGUI;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics
{
	Input Render::camera(Render::window);
	Shader GameObjectShader;
	Shader TestLightShader;

	void Render::RenderSetup()
	{
		Render::GLFWSetup();
		Render::WindowSetup();
		Render::GladSetup();

		Render::ContentSetup();

		EngineGUI::GetInstance().Initialize();

		Input::InputSetup();

		TimeManager::targetDT = 1.0f / EngineGUI::GetScreenRefreshRate();
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

		GameObjectShader = Shader(
			Engine::filesPath + "/shaders/GameObject.vert",
			Engine::filesPath + "/shaders/GameObject.frag");
		TestLightShader = Shader(
			Engine::filesPath + "/shaders/Light_Test.vert",
			Engine::filesPath + "/shaders/Light_Test.frag");

		float vertices[] = 
		{
			//positions          //normals           //texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};
		pointLightPositions[0] = vec3(1.5f, 0.0f, 1.5f);
		pointLightPositions[1] = vec3(1.5f, 0.0f, -1.5f);
		pointLightPositions[2] = vec3(-1.5f, 0.0f, 1.5f);
		pointLightPositions[3] = vec3(-1.5f, 0.0f, -1.5f);

		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);

		//position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glGenVertexArrays(1, &lightCubeVAO);
		glBindVertexArray(lightCubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		Texture tex(Engine::filesPath);
		tex.LoadTexture("textures/crate_2.png", false, GL_RGBA);
		tex.LoadTexture("textures/crate_2_specular.png", false, GL_RGBA);

		GameObjectShader.Use();
		GameObjectShader.SetInt("material.diffuse", 0);
		GameObjectShader.SetInt("material.specular", 1);

		UpdateAfterRescale(window, SCR_WIDTH, SCR_HEIGHT);
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Calculate the new aspect ratio
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		//Set the viewport based on the aspect ratio
		glViewport(0, 0, width, height);
	}

	void Render::WindowLoop()
	{
		glClearColor(
			backgroundColor.x,
			backgroundColor.y,
			backgroundColor.z,
			1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		WindowContentLoop();

		EngineGUI::GetInstance().Render();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	void Render::WindowContentLoop()
	{
		//camera transformation
		Input::ProcessInput(window);

		GameObjectShader.Use();
		GameObjectShader.SetVec3("viewPos", camera.GetCameraPosition());
		GameObjectShader.SetFloat("material.shininess", shininess);

		//directional light
		GameObjectShader.SetVec3("dirLight.direction", directionalDirection);
		GameObjectShader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("dirLight.diffuse", directionalDiffuse);
		GameObjectShader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		GameObjectShader.SetFloat("dirLight.intensity", directionalIntensity);
		//point light 1
		GameObjectShader.SetVec3("pointLights[0].position", pointLightPositions[0]);
		GameObjectShader.SetVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("pointLights[0].diffuse", pointDiffuse);
		GameObjectShader.SetVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("pointLights[0].constant", 1.0f);
		GameObjectShader.SetFloat("pointLights[0].linear", 0.09f);
		GameObjectShader.SetFloat("pointLights[0].quadratic", 0.032f);
		GameObjectShader.SetFloat("pointLights[0].intensity", pointIntensity);
		GameObjectShader.SetFloat("pointLights[0].distance", pointDistance);
		//point light 2
		GameObjectShader.SetVec3("pointLights[1].position", pointLightPositions[1]);
		GameObjectShader.SetVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("pointLights[1].diffuse", pointDiffuse);
		GameObjectShader.SetVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("pointLights[1].constant", 1.0f);
		GameObjectShader.SetFloat("pointLights[1].linear", 0.09f);
		GameObjectShader.SetFloat("pointLights[1].quadratic", 0.032f);
		GameObjectShader.SetFloat("pointLights[1].intensity", pointIntensity);
		GameObjectShader.SetFloat("pointLights[1].distance", pointDistance);
		//point light 3
		GameObjectShader.SetVec3("pointLights[2].position", pointLightPositions[2]);
		GameObjectShader.SetVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("pointLights[2].diffuse", pointDiffuse);
		GameObjectShader.SetVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("pointLights[2].constant", 1.0f);
		GameObjectShader.SetFloat("pointLights[2].linear", 0.09f);
		GameObjectShader.SetFloat("pointLights[2].quadratic", 0.032f);
		GameObjectShader.SetFloat("pointLights[2].intensity", pointIntensity);
		GameObjectShader.SetFloat("pointLights[2].distance", pointDistance);
		//point light 4
		GameObjectShader.SetVec3("pointLights[3].position", pointLightPositions[3]);
		GameObjectShader.SetVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		GameObjectShader.SetVec3("pointLights[3].diffuse", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("pointLights[3].constant", 1.0f);
		GameObjectShader.SetFloat("pointLights[3].linear", 0.09f);
		GameObjectShader.SetFloat("pointLights[3].quadratic", 0.032f);
		GameObjectShader.SetFloat("pointLights[3].intensity", pointIntensity);
		GameObjectShader.SetFloat("pointLights[3].distance", pointDistance);
		//spotLight
		GameObjectShader.SetVec3("spotLight.position", camera.GetCameraPosition());
		GameObjectShader.SetVec3("spotLight.direction", camera.GetFront());
		GameObjectShader.SetFloat("spotLight.intensity", spotIntensity);
		GameObjectShader.SetFloat("spotLight.distance", spotDistance);
		GameObjectShader.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		GameObjectShader.SetVec3("spotLight.diffuse", spotDiffuse);
		GameObjectShader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		GameObjectShader.SetFloat("spotLight.constant", 1.0f);
		GameObjectShader.SetFloat("spotLight.linear", 0.09f);
		GameObjectShader.SetFloat("spotLight.quadratic", 0.032f);
		GameObjectShader.SetFloat("spotLight.cutOff", cos(radians(spotInnerAngle)));
		GameObjectShader.SetFloat("spotLight.outerCutOff", cos(radians(spotOuterAngle)));

		//calculate the new projection matrix
		mat4 projection = perspective(
			radians(fov),
			aspectRatio,
			nearClip,
			farClip);

		//update the camera
		mat4 view = camera.GetViewMatrix();

		GameObjectShader.SetMat4("projection", projection);
		GameObjectShader.SetMat4("view", view);

		mat4 model = mat4(1.0f);

		//bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[0]);
		//bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[1]);

		static float cubeRotAngle = 0.0f;
		cubeRotAngle += cubeSpeedMultiplier / 500;
		float cubeX = lightPos.x + lampOrbitRange * cos(cubeRotAngle);
		float cubeZ = lightPos.z;
		float cubeY = lightPos.y + lampOrbitRange * sin(cubeRotAngle);
		float yOffset = cubeWiggleHeight * sin(cubeRotAngle * cubeWiggleSpeed);
		model = translate(model, vec3(cubeX, lightPos.y + yOffset, cubeY));
		//model = rotate(model, cubeRotAngle, vec3(0.0f, 1.0f, 0.0f));
		
		GameObjectShader.SetMat4("model", model);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//draw light sources
		TestLightShader.Use();
		TestLightShader.SetMat4("projection", projection);
		TestLightShader.SetMat4("view", view);
		TestLightShader.SetVec3("lightColor", pointDiffuse);

		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = mat4(1.0f);
			model = translate(model, pointLightPositions[i]);
			model = scale(model, vec3(0.2f));
			TestLightShader.SetMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}