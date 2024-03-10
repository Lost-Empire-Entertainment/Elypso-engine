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
using std::to_string;
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
		static ImVec2 lastBackgroundDragPos{};
		static ImVec2 lastNodeDragPos{};
		static bool isDraggingBackground = false;
		static bool isDraggingNode = false;

		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (renderNodeBlock
			&& ImGui::Begin("Nodeblock", NULL, windowFlags))
		{
			static float zoomFactor = 1.0f;
			static ImVec2 imagePos(0.0f, 0.0f);

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

			ImGuiChildFlags childWindowFlags = ImGuiWindowFlags_NoScrollbar;

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
					&& !isDraggingBackground)
				{
					isDraggingBackground = true;
					lastBackgroundDragPos = ImGui::GetMousePos();
				}
				if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{
					isDraggingBackground = false;
				}

				if (isDraggingBackground)
				{
					ImVec2 currentMousePos = ImGui::GetMousePos();
					ImVec2 screenDelta = ImVec2(
						currentMousePos.x - lastBackgroundDragPos.x,
						currentMousePos.y - lastBackgroundDragPos.y);

					imagePos.x += screenDelta.x;
					imagePos.y += screenDelta.y;

					if (selectedComponent != nullptr 
						&& selectedComponent->GetNodes().size() > 0)
					{
						for (auto& node : selectedComponent->GetNodes())
						{
							if (node == selectedNode)
							{
								ImVec2 initialPos = ImVec2(node->GetInitialPos().x, node->GetInitialPos().y);
								ImVec2 newNodePos = ImVec2(
									initialPos.x + screenDelta.x / zoomFactor,
									initialPos.y + screenDelta.y / zoomFactor);
								node->SetPos(vec2(newNodePos.x, newNodePos.y));
							}
							else
							{
								vec2 nodePos = node->GetPos();
								nodePos.x += screenDelta.x / zoomFactor;
								nodePos.y += screenDelta.y / zoomFactor;
								node->SetPos(nodePos);
							}
						}
					}

					lastBackgroundDragPos = currentMousePos;
				}
			}

			ImVec2 imageSize(5000 * zoomFactor, 5000 * zoomFactor);

			ImGui::SetCursorPos(imagePos);
			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(textureID)), imageSize);

			if (selectedComponent != nullptr)
			{
				if (!Input::cameraEnabled
					&& ImGui::IsWindowHovered()
					&& ImGui::IsMouseClicked(1))
				{
					cout << selectedComponent->GetName() << "\n";
					ImGui::OpenPopup("rightclickpopup");
				}

				if (ImGui::BeginPopupContextItem("rightclickpopup"))
				{
					if (ImGui::MenuItem("Add node"))
					{
						shared_ptr<Node> newNode = Node::InitializeNode();
						selectedComponent->AddNode(newNode);
						vec2 pos = vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
						newNode->SetPos(pos);

						cout << "added node " << newNode->GetName() << " to component " << selectedComponent->GetName() << "\n";
					}

					ImGui::EndPopup();
				}

				if (selectedComponent->GetNodes().size() > 0)
				{
					bool wasNodeSelected = false;

					for (const auto& node : selectedComponent->GetNodes())
					{
						string nodeWindowName = node->GetName() + to_string(node->GetID());
						string nodeWindowTitle = node->GetName() + " | " + to_string(node->GetID());
						ImVec2 nodePos = ImVec2(
							node->GetPos().x * zoomFactor,
							node->GetPos().y * zoomFactor);
						ImVec2 nodeSize = ImVec2(
							250 * zoomFactor, 
							180 * zoomFactor);

						ImGui::SetCursorPos(nodePos);

						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4 * zoomFactor, 4 * zoomFactor));
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8 * zoomFactor, 8 * zoomFactor));

						ImGui::BeginChild(nodeWindowName.c_str(), nodeSize);

						//draw background
						ImVec2 padding = ImVec2(5.0f, 5.0f);
						ImVec4 backgroundColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
						ImVec2 bgMin = ImVec2(
							ImGui::GetWindowPos().x - padding.x,
							ImGui::GetWindowPos().y - padding.y);
						ImVec2 bgMax = ImVec2(
							ImGui::GetWindowPos().x + nodeSize.x + padding.x,
							ImGui::GetWindowPos().y + nodeSize.y + padding.y);
						ImGui::GetWindowDrawList()->AddRectFilled(
							bgMin,
							bgMax,
							ImColor(backgroundColor),
							5.0f);

						ImGui::Text(nodeWindowTitle.c_str());
						ImGui::Separator();
						ImGui::Text("this is a node...");

						ImGui::EndChild();

						ImGui::PopStyleVar(2);

						if (ImGui::IsItemHovered()
							&& ImGui::IsItemClicked(0))
						{
							selectedNode = node;
							string nodeName = selectedNode->GetName() + " | " + to_string(selectedNode->GetID());
							cout << "selected " << nodeName << "\n";

							wasNodeSelected = true;
						}

						if (selectedNode == node
							&& ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
						{
							DestroyNode(selectedNode);
						}
					}

					if (ImGui::IsMouseClicked(0)
						&& !wasNodeSelected
						&& selectedNode != nullptr)
					{
						string nodeName = selectedNode->GetName() + " | " + to_string(selectedNode->GetID());
						selectedNode = nullptr;
						cout << "deselected " << nodeName << "\n";
					}

					if (selectedNode != nullptr
						&& ImGui::IsMouseDragging(ImGuiMouseButton_Left)
						&& !isDraggingNode)
					{
						isDraggingNode = true;
						lastNodeDragPos = ImGui::GetMousePos();
					}
					if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
					{
						isDraggingNode = false;
					}

					if (isDraggingNode)
					{
						selectedNode->SetInitialPos(selectedNode->GetPos());

						ImVec2 currentMousePos = ImGui::GetMousePos();
						ImVec2 screenDelta = ImVec2(
							currentMousePos.x - lastNodeDragPos.x,
							currentMousePos.y - lastNodeDragPos.y);

						vec2 nodePos = selectedNode->GetPos();
						nodePos.x += screenDelta.x / zoomFactor;
						nodePos.y += screenDelta.y / zoomFactor;
						selectedNode->SetPos(nodePos);

						lastNodeDragPos = currentMousePos;
					}
				}
			}

			ImGui::EndChild();

			ImGui::End();
		}
	}

	void GUINodeBlock::DestroyNode(const shared_ptr<Node>& node)
	{
		string nodeName = node->GetName();

		if (selectedComponent->GetNodes().size() > 0)
		{
			vector<shared_ptr<Node>>& nodes = selectedComponent->GetNodes();
			auto it = find(nodes.begin(), nodes.end(), node);
			if (it != nodes.end()) nodes.erase(it);
		}

		if (selectedNode == node) selectedNode = nullptr;

		cout << "deleted node " << nodeName << ", new node count is " << selectedComponent->GetNodes().size() << "\n";
	}
}