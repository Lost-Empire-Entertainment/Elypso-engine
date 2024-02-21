//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "type_ptr.hpp"
#include "magic_enum.hpp"

//engine
#include "gui_inspector.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "gameobject.hpp"
#include "sceneFile.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::to_string;

using Graphics::Render;
using Physics::Select;
using Graphics::Shape::GameObject;
using Type = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Transform;
using Graphics::Shape::Material;
using EngineFile::SceneFile;

namespace Graphics::GUI
{
	void GUIInspector::RenderInspector()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (renderInspector
			&& ImGui::Begin("Inpsector", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderInspector = false;
			}

			RSM_SelectedObject();

			ImGui::End();
		}
	}
	void GUIInspector::RSM_SelectedObject()
	{
		if (Select::isObjectSelected
			&& Select::selectedObj->IsInitialized())
		{
			shared_ptr<GameObject>& obj = Select::selectedObj;

			string objID = "ID: " + to_string(obj->GetID());
			ImGui::Text(objID.c_str());

			ImGui::SameLine();

			Type objType = obj->GetMesh()->GetMeshType();
			string objTypeValue = "     Type: " + string(magic_enum::enum_name(objType));
			ImGui::Text(objTypeValue.c_str());

			ImGui::Spacing();

			ImGui::Text("Name");
			string objName = obj->GetName();
			strcpy_s(inputTextBuffer_objName, bufferSize, objName.c_str());
			if (ImGui::InputText("##objName", inputTextBuffer_objName, bufferSize))
			{
				obj->SetName(inputTextBuffer_objName);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::Separator();

			ImGui::Text("Transform");

			ImGui::Spacing();

			vec3 pos = obj->GetTransform()->GetPosition();
			ImGui::Text("Position");
			if (ImGui::DragFloat3("##objPos", value_ptr(pos), 0.01f))
			{
				obj->GetTransform()->SetPosition(pos);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			vec3 rot = obj->GetTransform()->GetRotation();
			ImGui::Text("Rotation");
			if (ImGui::DragFloat3("##objRot", value_ptr(rot), 0.1f))
			{
				obj->GetTransform()->SetRotation(rot);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			if (rot.x > 359.99f 
				|| rot.x < -359.99f)
			{
				rot.x = 0.0f;
				obj->GetTransform()->SetRotation(rot);
			}
			if (rot.y > 359.99f 
				|| rot.y < -359.99f)
			{
				rot.y = 0.0f;
				obj->GetTransform()->SetRotation(rot);
			}
			if (rot.z > 359.99f 
				|| rot.z < -359.99f)
			{
				rot.z = 0.0f;
				obj->GetTransform()->SetRotation(rot);
			}

			vec3 scale = obj->GetTransform()->GetScale();
			ImGui::Text("Scale");
			if (ImGui::DragFloat3("##objScale", value_ptr(scale), 0.01f))
			{
				obj->GetTransform()->SetScale(scale);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			if (scale.x < 0.0f)
			{
				scale.x = 0.0f;
				obj->GetTransform()->SetScale(scale);
			}
			if (scale.y < 0.0f)
			{
				scale.y = 0.0f;
				obj->GetTransform()->SetScale(scale);
			}
			if (scale.z < 0.0f)
			{
				scale.z = 0.0f;
				obj->GetTransform()->SetScale(scale);
			}

			ImGui::Separator();

			ImGui::Text("Material");

			ImGui::Spacing();

			if (objType == Type::cube)
			{
				float cubeShininess = obj->GetBasicShape()->GetShininess();
				ImGui::Text("Shininess");
				if (ImGui::DragFloat("##shininess", &cubeShininess, 0.1f, 3.0f, 128.0f))
				{
					obj->GetBasicShape()->SetShininess(cubeShininess);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##shininess"))
				{
					obj->GetBasicShape()->SetShininess(32.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}
			else if (objType == Type::point_light)
			{
				vec3 pointDiffuse = obj->GetPointLight()->GetDiffuse();
				ImGui::Text("Point light diffuse");
				if (ImGui::ColorEdit3("##pointdiff", value_ptr(pointDiffuse)))
				{
					obj->GetPointLight()->SetDiffuse(pointDiffuse);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float pointIntensity = obj->GetPointLight()->GetIntensity();
				ImGui::Text("Point light intensity");
				if (ImGui::DragFloat("##pointint", &pointIntensity, 0.01f, 0.0f, 5.0f))
				{
					obj->GetPointLight()->SetIntensity(pointIntensity);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##pointint"))
				{
					obj->GetPointLight()->SetIntensity(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float pointDistance = obj->GetPointLight()->GetDistance();
				ImGui::Text("Point light distance");
				if (ImGui::DragFloat("##pointdist", &pointDistance, 0.1f, 0.0f, 25.0f))
				{
					obj->GetPointLight()->SetDistance(pointDistance);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##pointdist"))
				{
					obj->GetPointLight()->SetDistance(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}
			else if (objType == Type::spot_light)
			{
				vec3 spotDiffuse = obj->GetSpotLight()->GetDiffuse();
				ImGui::Text("Spotlight diffuse");
				if (ImGui::ColorEdit3("##spotdiff", value_ptr(spotDiffuse)))
				{
					obj->GetSpotLight()->SetDiffuse(spotDiffuse);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotIntensity = obj->GetSpotLight()->GetIntensity();
				ImGui::Text("Spotlight intensity");
				if (ImGui::DragFloat("##spotint", &spotIntensity, 0.01f, 0.0f, 5.0f))
				{
					obj->GetSpotLight()->SetIntensity(spotIntensity);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotint"))
				{
					obj->GetSpotLight()->SetIntensity(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotDistance = obj->GetSpotLight()->GetDistance();
				ImGui::Text("Spotlight distance");
				if (ImGui::DragFloat("##spotdist", &spotDistance, 0.1f, 0.0f, 25.0f))
				{
					obj->GetSpotLight()->SetDistance(spotDistance);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotdist"))
				{
					obj->GetSpotLight()->SetDistance(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotInnerAngle = obj->GetSpotLight()->GetInnerAngle();
				float spotOuterAngle = obj->GetSpotLight()->GetOuterAngle();

				ImGui::Text("Spotlight inner angle");
				if (ImGui::DragFloat("##spotinnerangle", &spotInnerAngle, 0.1f, 0.0f, spotOuterAngle - 0.01f))
				{
					obj->GetSpotLight()->SetInnerAngle(spotInnerAngle);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotinnerangle"))
				{
					obj->GetSpotLight()->SetInnerAngle(12.5f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				ImGui::Text("Spotlight outer angle");
				if (ImGui::DragFloat("##spotouterangle", &spotOuterAngle, 0.1f, spotInnerAngle + 0.01f, 50.0f))
				{
					obj->GetSpotLight()->SetOuterAngle(spotOuterAngle);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotouterangle"))
				{
					obj->GetSpotLight()->SetOuterAngle(17.5f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}
		}
	}
}