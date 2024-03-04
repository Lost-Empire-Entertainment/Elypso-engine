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

#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_scenehierarchy.hpp"
#include "gameobject.hpp"
#include "gui.hpp"
#include "sceneFile.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "input.hpp"

using std::cout;

using Graphics::Shape::Mesh;
using Graphics::Shape::GameObjectManager;
using Graphics::GUI::EngineGUI;
using EngineFile::SceneFile;
using Graphics::Render;
using Physics::Select;
using Core::Input;

namespace Graphics::GUI
{
	void GUISceneHierarchy::RenderSceneHierarchy()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (renderSceneHierarchy
			&& ImGui::Begin("Scene hierarchy", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderSceneHierarchy = false;
			}

			for (const auto& obj : GameObjectManager::GetObjects())
			{
				Mesh::MeshType type = obj->GetMesh()->GetMeshType();
				if (type != Mesh::MeshType::actionTex
					&& type != Mesh::MeshType::billboard
					&& type != Mesh::MeshType::border)
				{
					DrawGameObject(obj);
				}
			}
			RightClickPopup();
			RenameGameObject();

			ImGui::End();
		}
	}

	void GUISceneHierarchy::DrawGameObject(const shared_ptr<GameObject> obj)
	{
		ImGui::Selectable(obj->GetName().c_str(), false, ImGuiSelectableFlags_AllowItemOverlap);

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(0))
			{
				Select::selectedObj = obj;
				Select::isObjectSelected = true;
				Input::objectAction = Input::ObjectAction::move;
			}

			if (ImGui::IsMouseClicked(1))
			{
				selectedGameObject = obj;
				rightMouseClicked = true;
			}
		}
	}

	void GUISceneHierarchy::RightClickPopup()
	{
		if (rightMouseClicked)
		{
			ImGui::OpenPopup("rightclickpopup");
			rightMouseClicked = false;
		}

		if (ImGui::BeginPopupContextItem("rightclickpopup"))
		{
			if (ImGui::MenuItem("Create gameobject"))
			{
				cout << "Created new gameobject \n";

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Rename"))
			{
				isRenaming = true;
			}

			if (ImGui::MenuItem("Copy"))
			{
				copiedGameObject = selectedGameObject;

				cout << "Copied " << selectedGameObject->GetName() << "\n";
			}

			if (ImGui::MenuItem("Paste"))
			{
				cout << "Pasted " << copiedGameObject->GetName() << " to " << selectedGameObject->GetName() << "\n";

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Delete"))
			{
				string objName = selectedGameObject->GetName();

				GameObjectManager::DestroyGameObject(selectedGameObject);

				cout << "Deleted " << objName << "\n";

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::EndPopup();
		}
	}

	void GUISceneHierarchy::RenameGameObject()
	{
		if (isRenaming)
		{
			ImGui::OpenPopup("Rename gameobject");
			isRenaming = false;
		}

		if (ImGui::BeginPopupModal("Rename gameobject", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Enter new name:");
			ImGui::InputText("##newname", newNameBuffer, sizeof(newNameBuffer));

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				//check if the new name is not empty
				if (strlen(newNameBuffer) > 0)
				{
					selectedGameObject->SetName(newNameBuffer);
				}

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}