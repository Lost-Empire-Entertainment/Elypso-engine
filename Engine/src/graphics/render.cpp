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
#include "stringUtils.hpp"
#include "shader.hpp"

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
using Utils::String;

namespace Graphics
{
	Camera Render::camera(Render::window, 0.05f);
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;
	int framebufferWidth = 1280;
	int framebufferHeight = 720;

	void Render::RenderSetup()
	{
		GLFWSetup();
		WindowSetup();
		GladSetup();
		FramebufferSetup();

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
			1280, 
			720, 
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
			1280, 
			720);
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
				"Framebuffer is not complete!\n\n");
			return;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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
		//calculate the new aspect ratio
		//Camera::aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		//set the viewport based on the aspect ratio
		//glViewport(0, 0, width, height);
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
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//all windows, including RenderToImguiWindow 
		//with scene content are called in the Render function
		EngineGUI::Render();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void Render::RenderToImguiWindow()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(5000, 5000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (ImGui::Begin("Scene", NULL, windowFlags))
		{
			ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
			ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
			ImVec2 availableSize = ImVec2(
				contentRegionMax.x - contentRegionMin.x, 
				contentRegionMax.y - contentRegionMin.y);

			float windowAspectRatio = availableSize.x / availableSize.y;
			float targetAspectRatio = windowAspectRatio;

			if (aspectRatio[currentIndex] == "16:9")
				targetAspectRatio = 16.0f / 9.0f;
			else if (aspectRatio[currentIndex] == "16:10")
				targetAspectRatio = 16.0f / 10.0f;
			else if (aspectRatio[currentIndex] == "21:9")
				targetAspectRatio = 21.0f / 9.0f;
			else if (aspectRatio[currentIndex] == "32:9")
				targetAspectRatio = 32.0f / 9.0f;
			else if (aspectRatio[currentIndex] == "4:3")
				targetAspectRatio = 4.0f / 3.0f;

			ImVec2 renderSize = availableSize;
			if (windowAspectRatio > targetAspectRatio)
			{
				renderSize.x = availableSize.y * targetAspectRatio;
			}
			else if (windowAspectRatio < targetAspectRatio)
			{
				renderSize.y = availableSize.x / targetAspectRatio;
			}

			renderSize.x = roundf(renderSize.x);
			renderSize.y = roundf(renderSize.y);

			ImVec2 padding(
				(availableSize.x - renderSize.x) * 0.5f,
				(availableSize.y - renderSize.y) * 0.5f);
			ImGui::SetCursorPos(ImVec2(
				contentRegionMin.x + padding.x, 
				contentRegionMin.y + padding.y));

			if (renderSize.x != framebufferWidth
				|| renderSize.y != framebufferHeight)
			{
				framebufferWidth = static_cast<int>(renderSize.x);
				framebufferHeight = static_cast<int>(renderSize.y);

				glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RGB,
					framebufferWidth,
					framebufferHeight,
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					NULL);

				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
				glRenderbufferStorage(
					GL_RENDERBUFFER,
					GL_DEPTH24_STENCIL8,
					framebufferWidth,
					framebufferHeight);

				Camera::aspectRatio = targetAspectRatio;

				glViewport(0, 0, framebufferWidth, framebufferHeight);
			}

			isSceneSelected = ImGui::IsWindowFocused();
			isSceneHovered = ImGui::IsWindowHovered();
			if (isSceneSelected
				|| isSceneHovered)
			{
				ImGui::CaptureMouseFromApp(false);
				Input::SceneWindowInput();
			}
			else ImGui::ResetMouseDragDelta();

			//render to imgui image and flip the Y-axis
			ImGui::Image(
				(void*)(intptr_t)textureColorbuffer, 
				renderSize, 
				ImVec2(0, 1), 
				ImVec2(1, 0));

			ImGui::End();
		}
	}
}