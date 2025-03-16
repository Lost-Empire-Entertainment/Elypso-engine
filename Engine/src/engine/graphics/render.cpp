//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

//external
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "type_ptr.hpp"
#include "stb_image.h"

//engine
#include "console.hpp"
#include "core.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "timeManager.hpp"
#include "pointlight.hpp"
#include "gameobject.hpp"
#include "sceneFile.hpp"
#include "configFile.hpp"
#include "input.hpp"
#include "stringUtils.hpp"
#include "shader.hpp"
#include "selectobject.hpp"
#include "skybox.hpp"
#include "cameraobject.hpp"
#include "cameracomponent.hpp"
#if ENGINE_MODE
#include "compile.hpp"
#include "grid.hpp"
#include "selectedobjectaction.hpp"
#include "selectedobjectborder.hpp"
#include "gui_engine.hpp"
#else
#include "gui_game.hpp"
#endif

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
using std::vector;

using Core::Input;
using Core::TimeManager;
using Core::Engine;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::PointLight;
using Graphics::Shape::Skybox;
using EngineFile::SceneFile;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using EngineFile::ConfigFile;
using Utils::String;
using Core::Select;
using Graphics::Shape::CameraObject;
using Graphics::Components::CameraComponent;
#if ENGINE_MODE
using Core::Compilation;
using Graphics::Grid;
using Graphics::Shape::Border;
using Graphics::Shape::ActionTex;
using Graphics::GUI::EngineGUI;
#else
using Graphics::GUI::GameGUI;
#endif

namespace Graphics
{
#if ENGINE_MODE
	unsigned int framebuffer;
#endif

	void Render::RenderSetup()
	{
		GLFWSetup();
		WindowSetup();
		GladSetup();
#if ENGINE_MODE
		FramebufferSetup();
#endif
		ContentSetup();

#if ENGINE_MODE
		EngineGUI::Initialize();
#else
		GameGUI::Initialize();
#endif
		TimeManager::InitializeDeltaTime();

#if	ENGINE_MODE
#else
		GameObjectManager::renderBorders = false;
		GameObjectManager::renderPointLightBillboards = false;
		GameObjectManager::renderSpotlightBillboards = false;
		GameObjectManager::renderDirLightBillboard = false;
		GameObjectManager::renderAudioObjectBillboards = false;
		GameObjectManager::renderCameraObjectBillboards = false;
#endif
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
		string windowName = Engine::name + " " + Engine::version;
		window = glfwCreateWindow(
			1024,
			768,
			(windowName).c_str(),
			NULL,
			NULL);

		if (window == NULL)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INITIALIZE,
				Type::EXCEPTION,
				"Error: Failed to create GLFW window!\n\n");
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
		glfwSetWindowSizeLimits(window, 1024, 768, 7680, 4320);
		glfwSwapInterval(stoi(ConfigFile::GetValue("window_vsync")));

		int width, height, channels;
		string iconpath = (path(Engine::filesPath) / "icon.png").string();
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
				"Error: Failed to initialize GLAD!\n\n");
			return;
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::INITIALIZE,
			Type::DEBUG,
			"GLAD initialized successfully!\n\n");
	}
#if ENGINE_MODE
	void Render::FramebufferSetup()
	{
		//set up framebuffer
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		//set up color attachment texture
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_RGB, 
			1024, 
			768, 
			0, 
			GL_RGB, 
			GL_UNSIGNED_BYTE, 
			NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, 
			GL_COLOR_ATTACHMENT0, 
			GL_TEXTURE_2D, 
			textureColorbuffer, 
			0);

		//set up renderbuffer object 
		//for depth and stencil attachment
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(
			GL_RENDERBUFFER, 
			GL_DEPTH24_STENCIL8, 
			1024, 
			768);
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			GL_RENDERBUFFER, 
			rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::INITIALIZE,
				Type::EXCEPTION,
				"Error: Framebuffer is not complete!\n\n");
			return;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
#endif
	void Render::ContentSetup()
	{
		//enable face culling
		glEnable(GL_CULL_FACE);
		//cull back faces
		glCullFace(GL_BACK);
		//enable depth testing
		glEnable(GL_DEPTH_TEST);
		//enable blending
		glEnable(GL_BLEND);
		//set blending function
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if ENGINE_MODE
		Grid::InitializeGrid();

		shared_ptr<GameObject> border = Border::InitializeBorder();

		shared_ptr<GameObject> actionTex = ActionTex::InitializeActionTex();
#endif
		shared_ptr<GameObject> skybox = Skybox::InitializeSkybox();

		glfwMaximizeWindow(window);

#if	ENGINE_MODE
		/*
		string sceneCameraName = "SceneCamera";
		unsigned int nextID = ++GameObject::nextID;
		unsigned int nextID2 = ++GameObject::nextID;

		sceneCamera = CameraObject::InitializeCameraObject(
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, -90.0f),
			vec3(1.0f),
			"",
			sceneCameraName,
			nextID,
			true,
			nextID2,
			true);

		activeCamera = sceneCamera;
		*/
#endif
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
#ifndef ENGINE_MODE
		if (activeCamera != nullptr)
		{
			glViewport(0, 0, width, height);
			auto cc = activeCamera->GetComponent<CameraComponent>();
			cc->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
		}
#endif
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
		//camera transformation
		Input::ProcessKeyboardInput(window);

		if (activeCamera != nullptr)
		{
			//calculate the new projection matrix
			auto cc = Render::activeCamera->GetComponent<CameraComponent>();
			float fov = stof(ConfigFile::GetValue("camera_fov"));
			float nearClip = stof(ConfigFile::GetValue("camera_nearClip"));
			float farClip = stof(ConfigFile::GetValue("camera_farClip"));
			projection = perspective(
				radians(fov),
				cc->GetAspectRatio(),
				nearClip,
				farClip);

			//update the camera
			view = cc->GetViewMatrix();
		}

#if ENGINE_MODE
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#endif
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (activeCamera != nullptr
			&& GameObjectManager::GetSkybox() != nullptr)
		{
			mat4 nonConstView = view;
			Skybox::RenderSkybox(
				GameObjectManager::GetSkybox(), 
				nonConstView, 
				projection);

#if ENGINE_MODE

			glDepthMask(GL_FALSE);
			Grid::RenderGrid(view, projection);
			glDepthMask(GL_TRUE);
#endif
			GameObjectManager::RenderAll(view, projection);
		}

#if ENGINE_MODE
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		//all windows, including RenderToImguiWindow 
		//with scene content are called in the Render function
		EngineGUI::Render();
#else
		GameGUI::Render();
		Input::SceneWindowInput();
#endif

		//swap the front and back buffers
		glfwSwapBuffers(window);
		if (!Engine::IsUserIdle())
		{
			glfwPollEvents();
		}
		else glfwWaitEvents();
	}
}
