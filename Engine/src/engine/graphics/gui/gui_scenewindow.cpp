//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#if ENGINE_MODE
#include <memory>
#include <vector>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glad.h"
#include "type_ptr.hpp"

//engine
#include "gui_scenewindow.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "render.hpp"
#include "configFile.hpp"
#include "compile.hpp"
#include "selectobject.hpp"
#include "sceneFile.hpp"
#include "gameobject.hpp"
#include "stringUtils.hpp"
#include "timeManager.hpp"

using std::shared_ptr;
using std::vector;

using Graphics::Camera;
using Core::Input;
using Graphics::Render;
using EngineFile::ConfigFile;
using Core::Compilation;
using Core::Select;
using EngineFile::SceneFile;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::GameObject;
using Utils::String;
using Core::TimeManager;

namespace Graphics::GUI
{
	int framebufferWidth = 1280;
	int framebufferHeight = 720;

	void GUISceneWindow::RenderSceneWindow()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(5000, 5000));
		ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (ImGui::Begin("Scene", NULL, windowFlags))
		{
			GUISceneWindow::RenderSceneWindowMainContent();
			ImGui::End();
		}
	}

	void GUISceneWindow::RenderSceneWindowMainContent()
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

			glBindTexture(GL_TEXTURE_2D, Render::textureColorbuffer);
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

			glBindRenderbuffer(GL_RENDERBUFFER, Render::rbo);
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

				glfwGetCursorPos(Render::window, &x, &y);
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
			(void*)(intptr_t)Render::textureColorbuffer,
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
			if (showSceneWindowDebugMenu) RenderSceneWindowRightContent();

			if (showLeftCornerContent)
			{
				RenderSceneWindowLeftContent();

				if (Select::selectedObj != nullptr
					&& Select::isObjectSelected)
				{
					ImGui::SetCursorPos(ImVec2(350, 40));
					RenderSceneWindowActionButtons();
				}
			}
			else
			{
				if (Select::selectedObj != nullptr
					&& Select::isObjectSelected)
				{
					ImGui::SetCursorPos(ImVec2(45, 40));
					RenderSceneWindowActionButtons();
				}
			}
		}
	}

	void GUISceneWindow::RenderSceneWindowLeftContent()
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

	void GUISceneWindow::RenderSceneWindowRightContent()
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
			currentIndex = stoi(ConfigFile::GetValue("aspect_ratio"));
			string aspectRatioValue = aspectRatio[currentIndex];
			if (ImGui::Button(aspectRatioValue.c_str()))
			{
				currentIndex++;
				if (currentIndex >= aspectRatio.size())
				{
					currentIndex = 0;
				}
				ConfigFile::SetValue("aspect_ratio", to_string(currentIndex));
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Separator();

			ImGui::Text("Render billboards");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
			bool renderBillboards = GameObjectManager::renderBillboards;
			if (ImGui::Checkbox("##renderBillboards", &renderBillboards))
			{
				GameObjectManager::renderBillboards = renderBillboards;
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Text("Render light borders");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
			bool renderLightBorders = GameObjectManager::renderLightBorders;
			if (ImGui::Checkbox("##renderLightBorders", &renderLightBorders))
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

	void GUISceneWindow::RenderSceneWindowActionButtons()
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

	void GUISceneWindow::UpdateCounts()
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
}
#endif