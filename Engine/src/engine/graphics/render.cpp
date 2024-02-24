//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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

#include <string>
#include <iostream>
#include <filesystem>

//external
#include "glad.h"
#include "glfw3.h"
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "type_ptr.hpp"
#include "stb_image.h"

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
#include "cube.hpp"
#include "pointlight.hpp"
#include "gameobject.hpp"
#include "grid.hpp"
#include "selectedobjectaction.hpp"
#include "selectedobjectborder.hpp"
#include "sceneFile.hpp"

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
using Core::Engine;
using Utils::Search;
using Graphics::Shape::GameObjectManager;
using Graphics::GUI::EngineGUI;
using Graphics::Shape::Cube;
using Graphics::Shape::PointLight;
using Graphics::Grid;
using Graphics::Shape::ActionTex;
using Graphics::Shape::Border;
using EngineFile::SceneFile;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics
{
	Input Render::camera(Render::window);

	void Render::RenderSetup()
	{
		Render::GLFWSetup();
		Render::WindowSetup();
		Render::GladSetup();

		Render::ContentSetup();

		EngineGUI::GetInstance().Initialize();

		TimeManager::InitializeDeltaTime();
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
				"Failed to create GLFW window!\n\n");
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
		glfwSetWindowSizeLimits(window, 1280, 720, 7680, 4320);
		glfwSwapInterval(1);

		int width, height, channels;
		string iconpath = Engine::enginePath + "/icon.png";
		unsigned char* iconData = stbi_load(iconpath.c_str(), &width, &height, &channels, 4);

		GLFWimage icon{};
		icon.width = width;
		icon.height = height;
		icon.pixels = iconData;

		glfwSetWindowIcon(window, 1, &icon);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
		glfwSetScrollCallback(window, Input::ScrollCallback);
		glfwSetKeyCallback(window, Input::KeyCallback);

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { Engine::Shutdown(); });

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
				"Failed to initialize GLAD!\n\n");
			return;
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::GLAD,
			Type::DEBUG,
			"GLAD initialized successfully!\n\n");
	}
	void Render::ContentSetup()
	{
		//enable depth testing
		glEnable(GL_DEPTH_TEST);
		//enable blending
		glEnable(GL_BLEND);
		//set blending function
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//set depth function (optional, but typically set during initialization)
		//glDepthFunc(GL_LEQUAL);

		Grid::InitializeGrid();

		shared_ptr<GameObject> border = Border::InitializeBorder();
		GameObjectManager::SetBorder(border);
		GameObjectManager::AddOpaqueObject(border);

		shared_ptr<GameObject> actionTex = ActionTex::InitializeActionTex();
		GameObjectManager::SetActionTex(actionTex);
		GameObjectManager::AddTransparentObject(actionTex);

		UpdateAfterRescale(window, SCR_WIDTH, SCR_HEIGHT);
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Calculate the new aspect ratio
		Input::aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		//Set the viewport based on the aspect ratio
		glViewport(0, 0, width, height);
	}

	void Render::SetWindowNameAsUnsaved(bool state)
	{
		SceneFile::unsavedChanges = state;

		string newName = state == true
			? Engine::name + " " + Engine::version + "*"
			: Engine::name + " " + Engine::version;
		glfwSetWindowTitle(window, newName.c_str());
	}

	void Render::WindowLoop()
	{
		glClearColor(
			backgroundColor.x,
			backgroundColor.y,
			backgroundColor.z,
			1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//camera transformation
		Input::ProcessInput(window);

		//calculate the new projection matrix
		projection = perspective(
			radians(Input::fov),
			Input::aspectRatio,
			Input::nearClip,
			Input::farClip);

		//update the camera
		view = camera.GetViewMatrix();

		Grid::RenderGrid(view, projection);

		GameObjectManager::RenderAll(view, projection);

		EngineGUI::GetInstance().Render();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}