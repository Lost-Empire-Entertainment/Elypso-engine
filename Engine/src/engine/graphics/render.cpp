//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

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
	Camera Render::camera(Render::window, 0.05f);
#if ENGINE_MODE
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;
	int framebufferWidth = 1280;
	int framebufferHeight = 720;
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
				"Error: Failed to create GLFW window!\n\n");
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
		GameObjectManager::SetBorder(border);
		GameObjectManager::AddOpaqueObject(border);

		shared_ptr<GameObject> actionTex = ActionTex::InitializeActionTex();
		GameObjectManager::SetActionTex(actionTex);
		GameObjectManager::AddTransparentObject(actionTex);
#endif
		SkyboxSetup();

		glfwMaximizeWindow(window);
	}

	void Render::SkyboxSetup()
	{
		vec3 pos = camera.GetCameraPosition();
		vec3 rot = vec3(0);
		vec3 scale = vec3(1);

		string skyboxVert = Engine::filesPath + "\\shaders\\Skybox.vert";
		string skyboxFrag = Engine::filesPath + "\\shaders\\Skybox.frag";

		shared_ptr<GameObject> skybox = Skybox::InitializeSkybox(
			pos,
			rot,
			scale,
			skyboxVert,
			skyboxFrag);

		GameObjectManager::SetSkybox(skybox);
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
#ifndef ENGINE_MODE
		glViewport(0, 0, width, height);
		Camera::aspectRatio = static_cast<float>(width) / static_cast<float>(height);
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

#if ENGINE_MODE
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#endif
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (GameObjectManager::GetSkybox() != nullptr)
		{
			mat4 nonConstView = view;
			Skybox::RenderSkybox(
				GameObjectManager::GetSkybox(), 
				nonConstView, 
				projection);
		}

#if ENGINE_MODE
		glDepthMask(GL_FALSE);
		Grid::RenderGrid(view, projection);
		glDepthMask(GL_TRUE);
#endif
		GameObjectManager::RenderAll(view, projection);

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
		glfwPollEvents();
	}
#if ENGINE_MODE
	void Render::RenderToImguiWindow()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(5000, 5000));
		ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
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
			if (isSceneSelected)
			{
				ImGui::CaptureMouseFromApp(false);
				Input::SceneWindowInput();

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					double x, y;

					glfwGetCursorPos(window, &x, &y);
					ImVec2 scenePos = ImGui::GetWindowPos();
					ImVec2 sceneSize = ImGui::GetWindowSize();

					x -= scenePos.x;
					y -= scenePos.y;

					ImVec2 imagePos = ImVec2(
						contentRegionMin.x + padding.x,
						contentRegionMin.y + padding.y);

					x -= imagePos.x;
					y -= imagePos.y;

					//check if the click is within the bounds of the final rendered image
					if (x >= 0
						&& x <= renderSize.x
						&& y >= 0
						&& y <= renderSize.y)
					{
						Input::ObjectInteraction(renderSize.x, renderSize.y, x, y);
					}
				}
			}
			else ImGui::ResetMouseDragDelta();

			//render to imgui image and flip the Y-axis
			ImGui::Image(
				(void*)(intptr_t)textureColorbuffer, 
				renderSize, 
				ImVec2(0, 1), 
				ImVec2(1, 0));

			//makes sure none of the interactable scene window buttons are displayed
			//while game is being compiled
			if (!Compilation::renderBuildingWindow)
			{
				ImVec2 windowStateBtnSize = ImVec2(25.0f, 25.0f);
				float windowStateBtnRightPadding = 10.0f;
				ImVec2 windowStateBtnPos(
					ImGui::GetWindowSize().x - windowStateBtnSize.x - windowStateBtnRightPadding, 40.0f);
				ImGui::SetCursorPos(windowStateBtnPos);
				if (ImGui::Button("X", windowStateBtnSize))
				{
					ConfigFile::SetValue("gui_sceneWindow", "0");
				}

				ImVec2 rightToggleBtnSize = ImVec2(25.0f, 25.0f);
				float rightToggleBtnRightPadding = 45.0f;
				ImVec2 rightToggleBtnPos(
					ImGui::GetWindowSize().x - rightToggleBtnSize.x - rightToggleBtnRightPadding, 40.0f);
				string rightToggleBtnText = showSceneWindowDebugMenu
					? "v##right" : "<##right";
				ImGui::SetCursorPos(rightToggleBtnPos);
				if (ImGui::Button(rightToggleBtnText.c_str(), rightToggleBtnSize))
				{
					showSceneWindowDebugMenu = !showSceneWindowDebugMenu;
				}

				ImVec2 leftToggleBtnSize = ImVec2(25.0f, 25.0f);
				float leftToggleBtnRightPadding = 10.0f;
				ImVec2 leftToggleBtnPos(leftToggleBtnRightPadding, 40.0f);
				string leftToggleBtnText = showLeftCornerContent
					? "v##left" : ">##left";
				ImGui::SetCursorPos(leftToggleBtnPos);
				if (ImGui::Button(leftToggleBtnText.c_str(), leftToggleBtnSize))
				{
					showLeftCornerContent = !showLeftCornerContent;
				}
			}

			//scene window buttons and interactables are not rendered 
			//if window size is smaller than minimum required
			ImVec2 windowSize = ImGui::GetWindowSize();

			int debugWindowAddition = showSceneWindowDebugMenu
				? 0
				: 400;
			int leftCornerContentAddition = showLeftCornerContent
				? 0
				: 400;
			bool actionButtonsTrue = Select::selectedObj != nullptr;
			int actionButtonsAddition = actionButtonsTrue
				? 0
				: 400;
			int windowSizeXLimit = 1200 - debugWindowAddition - leftCornerContentAddition - actionButtonsAddition;

			if (windowSize.x >= windowSizeXLimit
				&& windowSize.y > 400
				&& !Compilation::renderBuildingWindow)
			{
				if (showSceneWindowDebugMenu) SceneWindowDebugMenu();

				if (showLeftCornerContent)
				{
					SceneWindowLeftCornerContent();

					if (Select::selectedObj != nullptr
						&& Select::isObjectSelected)
					{
						ImGui::SetCursorPos(ImVec2(350, 40));
						SceneWindowActionButtons();
					}
				}
				else
				{
					if (Select::selectedObj != nullptr
						&& Select::isObjectSelected)
					{
						ImGui::SetCursorPos(ImVec2(45, 40));
						SceneWindowActionButtons();
					}
				}
			}

			ImGui::End();
		}
	}

	void Render::SceneWindowDebugMenu()
	{
		ImVec2 childSize = ImVec2(300.0f, 300.0f);
		float rightPadding = 80.0f;
		ImVec2 childPos(
			ImGui::GetWindowSize().x - childSize.x - rightPadding, 40.0f);
		ImGui::SetCursorPos(childPos);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));

		if (ImGui::BeginChild("##sceneWindowDebugMenu", childSize))
		{
			//
			// RENDER SETTINGS
			//

			ImGui::Text("Toggle VSync");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
			bool vsyncEnabled = stoi(ConfigFile::GetValue("window_vsync"));
			if (ImGui::Checkbox("##vsync", &vsyncEnabled))
			{
				glfwSwapInterval(vsyncEnabled ? 1 : 0);
				ConfigFile::SetValue("window_vsync", to_string(vsyncEnabled));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Toggle aspect ratio");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
			Render::currentIndex = stoi(ConfigFile::GetValue("aspect_ratio"));
			string aspectRatioValue = Render::aspectRatio[Render::currentIndex];
			if (ImGui::Button(aspectRatioValue.c_str()))
			{
				Render::currentIndex++;
				if (Render::currentIndex >= Render::aspectRatio.size())
				{
					Render::currentIndex = 0;
				}
				ConfigFile::SetValue("aspect_ratio", to_string(Render::currentIndex));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Render billboards");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 150);
			bool renderBillboards = GameObjectManager::renderBillboards;
			if (ImGui::Checkbox("##renderBillboards", &renderBillboards))
			{
				GameObjectManager::renderBillboards = renderBillboards;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Render light borders");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 200);
			bool renderLightBorders = GameObjectManager::renderLightBorders;
			if (ImGui::Checkbox("##renderLightBorders", &renderBillboards))
			{
				GameObjectManager::renderLightBorders = renderLightBorders;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			//
			// CAMERA SETTINGS
			//

			ImGui::Separator();

			ImGui::Text("FOV");
			float fov = stof(ConfigFile::GetValue("camera_fov"));
			if (ImGui::DragFloat("##fov", &fov, 0.1f, 70.0f, 110.0f))
			{
				if (fov > 110.0f) fov = 110.0f;
				if (fov < 70.0f) fov = 70.0f;

				ConfigFile::SetValue("camera_fov", to_string(fov));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Camera near clip");
			float nearClip = stof(ConfigFile::GetValue("camera_nearClip"));
			float farClip = stof(ConfigFile::GetValue("camera_farClip"));
			if (ImGui::DragFloat("##camNearClip", &nearClip, 0.1f, 0.001f, farClip - 0.001f))
			{
				if (nearClip > farClip - 0.001f) nearClip = farClip - 0.001f;
				if (nearClip > 0.5f) nearClip = 0.5f;
				if (nearClip < 0.001f) nearClip = 0.001f;

				ConfigFile::SetValue("camera_nearClip", to_string(nearClip));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Camera far clip");
			if (ImGui::DragFloat("##camFarClip", &farClip, 0.1f, nearClip + 0.001f, 10000))
			{
				if (farClip > 10000.0f) farClip = 10000.0f;
				if (farClip < nearClip + 0.001f) farClip = nearClip + 0.001f;
				if (farClip < 50.0f) farClip = 50.0f;

				ConfigFile::SetValue("camera_farClip", to_string(farClip));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Camera move speed multiplier");
			float moveSpeed = stof(ConfigFile::GetValue("camera_speedMultiplier"));
			if (ImGui::DragFloat("##camMoveSpeed", &moveSpeed, 0.1f, 0.1f, 100.0))
			{
				if (moveSpeed > 100.0f) moveSpeed = 100.0f;
				if (moveSpeed < 0.1f) moveSpeed = 0.1f;

				ConfigFile::SetValue("camera_speedMultiplier", to_string(moveSpeed));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##camMoveSpeed"))
			{
				ConfigFile::SetValue("camera_speedMultiplier", "1.0");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			//
			// GRID SETTINGS
			//

			ImGui::Separator();

			ImGui::Text("Grid color");
			string gridColorString = ConfigFile::GetValue("grid_color");
			vector<string> gridColorSplit = String::Split(gridColorString, ',');
			vec3 gridColor = vec3(
				stof(gridColorSplit[0]),
				stof(gridColorSplit[1]),
				stof(gridColorSplit[2]));
			if (ImGui::ColorEdit3("##gridColor", value_ptr(gridColor)))
			{
				string finalString =
					to_string(gridColor[0]) + "," +
					to_string(gridColor[1]) + "," +
					to_string(gridColor[2]);

				ConfigFile::SetValue("grid_color", finalString);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##gridColor"))
			{
				ConfigFile::SetValue("grid_color", "0.4, 0.4, 0.4");
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Grid transparency");
			float gridTransparency = stof(ConfigFile::GetValue("grid_transparency"));
			if (ImGui::DragFloat("##gridTransparency", &gridTransparency, 0.001f, 0.0f, 1.0f))
			{
				if (gridTransparency > 1.0f) gridTransparency = 1.0f;
				if (gridTransparency < 0.0f) gridTransparency = 0.0f;

				ConfigFile::SetValue("grid_transparency", to_string(gridTransparency));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Grid max distance");
			float gridMaxDistance = stof(ConfigFile::GetValue("grid_maxDistance"));

			if (gridMaxDistance > farClip)
			{
				gridMaxDistance = farClip;
				ConfigFile::SetValue("grid_maxDistance", to_string(gridMaxDistance));
			}
			if (ImGui::DragFloat("##gridMaxDistance", &gridMaxDistance, 0.1f, 10.0f, 200.0f))
			{
				if (gridMaxDistance > 200.0f) gridMaxDistance = 200.0f;
				if (gridMaxDistance < 10.0f) gridMaxDistance = 10.0f;

				ConfigFile::SetValue("grid_maxDistance", to_string(gridMaxDistance));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::EndChild();
		}

		ImGui::PopStyleColor();
	}

	void Render::SceneWindowLeftCornerContent()
	{
		ImVec2 childSize = ImVec2(300.0f, 150.0f);
		float leftPadding = 45.0f;
		ImVec2 childPos(leftPadding, 40.0f);
		ImGui::SetCursorPos(childPos);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));

		if (ImGui::BeginChild("##sceneWindowLeftCornerContent", childSize))
		{
			ImGui::Text("FPS: %.2f", TimeManager::displayedFPS);

			string strObjectsCount = "Objects: " + to_string(objectsCount);
			ImGui::Text(strObjectsCount.c_str());
			string strVerticesCount = "Vertices: " + to_string(verticesCount);
			ImGui::Text(strVerticesCount.c_str());

			ImGui::Text(
				"Position: %.2f, %.2f, %.2f",
				Render::camera.GetCameraPosition().x,
				Render::camera.GetCameraPosition().y,
				Render::camera.GetCameraPosition().z);
			ImGui::Text(
				"Angle: %.2f, %.2f, %.2f",
				Render::camera.GetCameraRotation().x,
				Render::camera.GetCameraRotation().y,
				Render::camera.GetCameraRotation().z);

			ImGui::EndChild();
		}

		ImGui::PopStyleColor();
	}

	void Render::SceneWindowActionButtons()
	{
		ImVec2 actionButtonSize = ImVec2(80, 30);
		ImVec2 axisButtonSize = ImVec2(30, 30);

		{
			bool isSelected = (Input::objectAction == Input::ObjectAction::move);
			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
			}
			if (ImGui::Button("Move", actionButtonSize))
			{
				Input::objectAction = Input::ObjectAction::move;
			}
			if (isSelected)
			{
				ImGui::PopStyleColor();
			}
		}

		ImGui::SameLine();
		{
			bool isSelected = (Input::objectAction == Input::ObjectAction::rotate);
			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
			}
			if (ImGui::Button("Rotate", actionButtonSize))
			{
				Input::objectAction = Input::ObjectAction::rotate;
			}
			if (isSelected)
			{
				ImGui::PopStyleColor();
			}
		}

		ImGui::SameLine();
		{
			bool isSelected = (Input::objectAction == Input::ObjectAction::scale);
			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
			}
			if (ImGui::Button("Scale", actionButtonSize))
			{
				Input::objectAction = Input::ObjectAction::scale;
			}
			if (isSelected)
			{
				ImGui::PopStyleColor();
			}
		}

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(20.0f, 0.0f));

		ImGui::SameLine();

		{
			bool isSelected = (Input::axis == "X");
			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
			}
			if (ImGui::Button("X##axis", axisButtonSize))
			{
				Input::axis = "X";
			}
			if (isSelected)
			{
				ImGui::PopStyleColor();
			}
		}

		ImGui::SameLine();
		{
			bool isSelected = (Input::axis == "Y");
			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
			}
			if (ImGui::Button("Y##axis", axisButtonSize))
			{
				Input::axis = "Y";
			}
			if (isSelected)
			{
				ImGui::PopStyleColor();
			}
		}

		ImGui::SameLine();
		{
			bool isSelected = (Input::axis == "Z");
			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
			}
			if (ImGui::Button("Z##axis", axisButtonSize))
			{
				Input::axis = "Z";
			}
			if (isSelected)
			{
				ImGui::PopStyleColor();
			}
		}
	}

	void Render::UpdateCounts()
	{
		if (!waitBeforeCountsUpdate)
		{
			objectsCount = static_cast<int>(GameObjectManager::GetObjects().size());

			verticesCount = 0;
			for (const shared_ptr<GameObject>& obj : GameObjectManager::GetObjects())
			{
				verticesCount += static_cast<int>(obj->GetMesh()->GetVertices().size());
			}
		}
	}
#endif
}