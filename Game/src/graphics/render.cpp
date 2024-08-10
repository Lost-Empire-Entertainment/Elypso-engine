//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>

//external
#include "stb_image.h"
#include "glad.h"
#include "type_ptr.hpp"

//game
#include "render.hpp"
#include "timemanager.hpp"
#include "gui.hpp"
#include "console.hpp"
#include "core.hpp"
#include "configfile.hpp"
#include "input.hpp"
#include "gameobject.hpp"

using glm::perspective;
using std::cout;

using Core::TimeManager;
using Graphics::GUI::GameGUI;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Game;
using GameFile::ConfigFile;
using Core::Input;
using Graphics::Shape::GameObjectManager;

namespace Graphics
{
	Camera Render::camera(window, 0.05f);

	void Render::Initialize()
	{
		GLFWSetup();
		WindowSetup();
		GladSetup();

		ContentSetup();

		GameGUI::Initialize();

		TimeManager::InitializeDeltaTime();
	}

	void Render::GLFWSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::INITIALIZE,
			Type::DEBUG,
			"Initializing GLFW...\n");

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		ConsoleManager::WriteConsoleMessage(
			Caller::INITIALIZE,
			Type::DEBUG,
			"GLFW initialized successfully!\n\n");
	}

	void Render::WindowSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::INITIALIZE,
			Type::DEBUG,
			"Creating window...\n");

		//create a window object holding all the windowing data
		window = glfwCreateWindow(
			1280,
			720,
			(Game::gameName).c_str(),
			NULL,
			NULL);

		if (window == NULL)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INITIALIZE,
				Type::EXCEPTION,
				"Failed to create GLFW window!\n\n");
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
		glfwSetWindowSizeLimits(window, 1280, 720, 7680, 4320);
		glfwSwapInterval(stoi(ConfigFile::GetValue("window_vsync")));

		int width, height, channels;
		string iconpath = Game::filesPath + "\\icon.png";
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
		glfwSetCursorPosCallback(window, Input::MouseMovementCallback);

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { Game::Shutdown(); });

		ConsoleManager::WriteConsoleMessage(
			Caller::INITIALIZE,
			Type::DEBUG,
			"Window initialized successfully!\n\n");
	}

	void Render::GladSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::INITIALIZE,
			Type::DEBUG,
			"Initializing GLAD...\n");

		//check if glad is initialized before continuing
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INITIALIZE,
				Type::EXCEPTION,
				"Failed to initialize GLAD!\n\n");
			return;
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::INITIALIZE,
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

		glfwMaximizeWindow(window);
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Calculate the new aspect ratio
		Camera::aspectRatio = static_cast<float>(width) / static_cast<float>(height);

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

		//camera transformation
		Input::ProcessKeyboardInput(window);

		//calculate the new projection matrix
		float fov = stof(ConfigFile::GetValue("camera_fov"));
		float nearClip = stof(ConfigFile::GetValue("camera_nearClip"));
		float farClip = stof(ConfigFile::GetValue("camera_farClip"));
		projection = perspective(
			radians(fov),
			Camera::aspectRatio,
			nearClip,
			farClip);

		//update the camera
		view = camera.GetViewMatrix();

		GameObjectManager::RenderAll(view, projection);

		GameGUI::Render();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}