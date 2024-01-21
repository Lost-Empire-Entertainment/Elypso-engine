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
			}

			ImGui::Separator();

			ImGui::Text("Transform");

			ImGui::Spacing();

			vec3 pos = obj->GetTransform()->GetPosition();
			ImGui::Text("Position");
			if (ImGui::InputFloat3("##objPos", value_ptr(pos)))
			{
				obj->GetTransform()->SetPosition(pos);
			}

			vec3 rot = obj->GetTransform()->GetRotation();
			ImGui::Text("Rotation");
			if (ImGui::InputFloat3("##objRot", value_ptr(rot)))
			{
				obj->GetTransform()->SetRotation(rot);
			}

			vec3 scale = obj->GetTransform()->GetScale();
			ImGui::Text("Scale");
			if (ImGui::InputFloat3("##objScale", value_ptr(scale)))
			{
				obj->GetTransform()->SetScale(scale);
			}

			ImGui::Separator();

			ImGui::Text("Material");

			ImGui::Spacing();

			if (objType == Type::cube)
			{
				float cubeShininess = obj->GetBasicShape()->GetShininess();
				ImGui::Text("Shininess");
				if (ImGui::SliderFloat("##shininess", &cubeShininess, 3.0f, 128.0f))
				{
					obj->GetBasicShape()->SetShininess(cubeShininess);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##shininess"))
				{
					obj->GetBasicShape()->SetShininess(32.0f);
				}
			}
			else if (objType == Type::point_light)
			{
				vec3 pointDiffuse = obj->GetPointLight()->GetDiffuse();
				ImGui::Text("Point light diffuse");
				if (ImGui::ColorEdit3("##pointdiff", value_ptr(pointDiffuse)))
				{
					obj->GetPointLight()->SetDiffuse(pointDiffuse);
				}

				float pointIntensity = obj->GetPointLight()->GetIntensity();
				ImGui::Text("Point light intensity");
				if (ImGui::SliderFloat("##pointint", &pointIntensity, 0.0f, 5.0f))
				{
					obj->GetPointLight()->SetIntensity(pointIntensity);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##pointint"))
				{
					obj->GetPointLight()->SetIntensity(1.0f);
				}

				float pointDistance = obj->GetPointLight()->GetDistance();
				ImGui::Text("Point light distance");
				if (ImGui::SliderFloat("##pointdist", &pointDistance, 0.0f, 25.0f))
				{
					obj->GetPointLight()->SetDistance(pointDistance);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##pointdist"))
				{
					obj->GetPointLight()->SetDistance(1.0f);
				}
			}
			else if (objType == Type::spot_light)
			{
				vec3 spotDiffuse = obj->GetSpotLight()->GetDiffuse();
				ImGui::Text("Spotlight diffuse");
				if (ImGui::ColorEdit3("##spotdiff", value_ptr(spotDiffuse)))
				{
					obj->GetSpotLight()->SetDiffuse(spotDiffuse);
				}

				float spotIntensity = obj->GetSpotLight()->GetIntensity();
				ImGui::Text("Spotlight intensity");
				if (ImGui::SliderFloat("##spotint", &spotIntensity, 0.0f, 5.0f))
				{
					obj->GetSpotLight()->SetIntensity(spotIntensity);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotint"))
				{
					obj->GetSpotLight()->SetIntensity(1.0f);
				}

				float spotDistance = obj->GetSpotLight()->GetDistance();
				ImGui::Text("Spotlight distance");
				if (ImGui::SliderFloat("##spotdist", &spotDistance, 0.0f, 25.0f))
				{
					obj->GetSpotLight()->SetDistance(spotDistance);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotdist"))
				{
					obj->GetSpotLight()->SetDistance(1.0f);
				}

				float spotInnerAngle = obj->GetSpotLight()->GetInnerAngle();
				float spotOuterAngle = obj->GetSpotLight()->GetOuterAngle();
				ImGui::Text("Spotlight inner angle");
				if (ImGui::SliderFloat("##spotinnerangle", &spotInnerAngle, 0.0f, spotOuterAngle - 0.01f))
				{
					obj->GetSpotLight()->SetInnerAngle(spotInnerAngle);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotinnerangle"))
				{
					obj->GetSpotLight()->SetInnerAngle(12.5f);
				}

				ImGui::Text("Spotlight outer angle");
				if (ImGui::SliderFloat("##spotouterangle", &spotInnerAngle, spotOuterAngle + 0.01f, 50.0f))
				{
					obj->GetSpotLight()->SetOuterAngle(spotOuterAngle);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotouterangle"))
				{
					obj->GetSpotLight()->SetOuterAngle(17.5f);
				}
			}
		}
	}
}