//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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

//game
#include "core.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "timeManager.hpp"
#include "pointlight.hpp"
#include "gameobject.hpp"
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
using Core::Game;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::PointLight;
using GameFile::SceneFile;

namespace Graphics
{
	Input Render::camera(Render::window);

	void Render::RenderSetup()
	{
		Render::GLFWSetup();
		Render::WindowSetup();
		Render::GladSetup();

		Render::ContentSetup();

		TimeManager::InitializeDeltaTime();
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
		window = glfwCreateWindow(
			windowWidth,
			windowHeight,
			(Game::name + " " + Game::version).c_str(),
			NULL,
			NULL);

		if (window == NULL)
		{
			cout << "Error: Failed to create GLFW window!\n\n";
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
		glfwSetWindowSizeLimits(window, 1280, 720, 7680, 4320);
		glfwSwapInterval(1);

		int width, height, channels;
		string iconpath = Game::filesPath + "/icon.png";
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

		UpdateAfterRescale(window, windowWidth, windowHeight);
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
			? Game::name + " " + Game::version + "*"
			: Game::name + " " + Game::version;
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
		Input::ProcessKeyboardInput(window);

		//calculate the new projection matrix
		projection = perspective(
			radians(Input::fov),
			Input::aspectRatio,
			Input::nearClip,
			Input::farClip);

		//update the camera
		view = camera.GetViewMatrix();

		GameObjectManager::RenderAll(view, projection);

		//swap the front and back buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}