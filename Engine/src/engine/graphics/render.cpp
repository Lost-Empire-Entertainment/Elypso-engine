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

//engine
#include "console.hpp"
#include "core.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "timeManager.hpp"
#include "pointlight.hpp"
#include "gameobject.hpp"
#include "grid.hpp"
#include "selectedobjectaction.hpp"
#include "selectedobjectborder.hpp"
#include "sceneFile.hpp"
#include "configFile.hpp"
#include "input.hpp"

using glm::perspective;
using glm::radians;
using glm::translate;
using glm::rotate;
using std::filesystem::exists;
using std::filesystem::current_path;
using std::cout;
using std::endl;
using std::to_string;
using std::filesystem::path;

using Core::Input;
using Core::TimeManager;
using Core::Engine;
using Graphics::Shape::GameObjectManager;
using Graphics::GUI::EngineGUI;
using Graphics::Shape::PointLight;
using Graphics::Grid;
using Graphics::Shape::ActionTex;
using Graphics::Shape::Border;
using EngineFile::SceneFile;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using EngineFile::ConfigFile;

namespace Graphics
{
	Camera Render::camera(Render::window, 0.05f);

   void GLAPIENTRY GLErrorCallback( GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar* message,
                                    const void* userParam)
   {
#if 1 // replace this with however you check that your compiling with DEBUG, or make it an int that you can change in the editor or something like that 
      std::cout << "GL ERROR : TYPE " << type <<" : SEVERITY " << severity << " : MESSAGE " << message << "\n"; 
#endif 
   }

	void Render::RenderSetup()
	{
		GLFWSetup();
		WindowSetup();
		GladSetup();

		ContentSetup();

		EngineGUI::Initialize();

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
			(Engine::name + " " + Engine::version).c_str(),
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
		string iconpath = Engine::filesPath + "\\icon.png";
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

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { Engine::Shutdown(); });

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

      //get gl version for debug callback
      int minor, major;
      glGetIntegerv(GL_MINOR_VERSION, &minor);
      glGetIntegerv(GL_MAJOR_VERSION, &major);
      //enable debug callback if opengl version is high enough
      if (major >= 4 && minor >= 3) {
         glEnable(GL_DEBUG_OUTPUT);
         glDebugMessageCallback(MessageCallback, 0);
      }


		Grid::InitializeGrid();

		shared_ptr<GameObject> border = Border::InitializeBorder();
		GameObjectManager::SetBorder(border);
		GameObjectManager::AddOpaqueObject(border);

		shared_ptr<GameObject> actionTex = ActionTex::InitializeActionTex();
		GameObjectManager::SetActionTex(actionTex);
		GameObjectManager::AddTransparentObject(actionTex);

		glfwMaximizeWindow(window);
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Calculate the new aspect ratio
		Camera::aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		//Set the viewport based on the aspect ratio
		glViewport(0, 0, width, height);
	}

	void Render::SetWindowNameAsUnsaved(bool state)
	{
		SceneFile::unsavedChanges = state;
		
		string sceneName = path(Engine::scenePath).parent_path().stem().string();
		string windowTitle = 
			Engine::name + " " 
			+ Engine::version 
			+ " [" + sceneName + "]";

		string newName = state == true
			? windowTitle + " *"
			: windowTitle;

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

		Grid::RenderGrid(view, projection);

		GameObjectManager::RenderAll(view, projection);

		EngineGUI::Render();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
