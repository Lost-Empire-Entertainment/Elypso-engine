//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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

#include <filesystem>
#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "stb_image.h"
#include "glfw3.h"
#include "glad.h"

//engine
#include "gui_nodeblock.hpp"
#include "gui.hpp"
#include "core.hpp"
#include "input.hpp"

using std::cout;
using std::max;
using std::min;
using std::filesystem::exists;

using Core::Engine;
using Core::Input;

namespace Graphics::GUI
{
	void GUINodeBlock::SetBackgroundTexture()
	{
		string backgroundPath = Engine::enginePath + "/textures/node_background.png";
		if (!exists(backgroundPath))
		{
			Engine::CreateErrorPopup("Node window setup error", "Couldn't find node window background texture!");
			return;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//load image, create texture and generate mipmaps
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load((backgroundPath).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			textureID = texture;
		}
		else Engine::CreateErrorPopup("Node window setup error", "Couldn't set up node window background texture!");
		stbi_image_free(data);
	}

	void GUINodeBlock::RenderNodeBlock()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (renderNodeBlock
			&& ImGui::Begin("Nodeblock", NULL, windowFlags))
		{
			static float zoomFactor = 1.0f;
			static ImVec2 imagePos(0.0f, 0.0f);
			static ImVec2 lastMousePos;
			static bool isDragging = false;

			if (selectedGameObject != nullptr
				&& selectedComponent != nullptr)
			{
				string text = selectedGameObject->GetName() + " | " + selectedComponent->GetName();
				ImGui::Text(text.c_str());
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderNodeBlock = false;
			}

			ImGuiChildFlags childWindowFlags = 
				ImGuiWindowFlags_HorizontalScrollbar
				| ImGuiWindowFlags_AlwaysHorizontalScrollbar;

			ImGui::BeginChild("NodeblockScrolling", ImVec2(0, 0), true, childWindowFlags);

			if (!Input::cameraEnabled
				&& ImGui::IsWindowHovered())
			{
				//zooming with mouse scroll wheel and ctrl key
				if (ImGui::GetIO().KeyCtrl
					&& ImGui::GetIO().MouseWheel != 0)
				{
					zoomFactor += ImGui::GetIO().MouseWheel * 0.1f;
					zoomFactor = max(zoomFactor, 0.3f);
					zoomFactor = min(zoomFactor, 5.0f);
				}

				//dragging with left mouse key
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)
					&& !isDragging)
				{
					isDragging = true;
					lastMousePos = ImGui::GetMousePos();
				}
				if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{
					isDragging = false;
				}

				if (isDragging)
				{
					ImVec2 mouseDelta;
					mouseDelta.x = ImGui::GetMousePos().x - lastMousePos.x;
					mouseDelta.y = ImGui::GetMousePos().y - lastMousePos.y;
					imagePos.x += mouseDelta.x;
					imagePos.y += mouseDelta.y;
					lastMousePos = ImGui::GetMousePos();
				}
			}

			ImVec2 imageSize(5000 * zoomFactor, 5000 * zoomFactor);

			ImGui::SetCursorPos(imagePos);
			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(textureID)), imageSize);

			if (!Input::cameraEnabled
				&& ImGui::IsWindowHovered()
				&& ImGui::IsMouseClicked(1))
			{
				ImGui::OpenPopup("rightclickpopup");
			}

			if (ImGui::BeginPopupContextItem("rightclickpopup"))
			{
				if (ImGui::MenuItem("Add node"))
				{

				}

				ImGui::EndPopup();
			}

			ImGui::EndChild();

			ImGui::End();
		}
	}
}