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
using Type = Graphics::Shape::Mesh::Type;
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
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			window->Size = ImClamp(window->Size, EngineGUI::minSize, EngineGUI::maxSize);

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderInspector = false;
			}

			if (ImGui::BeginTabBar("Inspector"))
			{
				if (ImGui::BeginTabItem("Selected"))
				{
					RSM_SelectedObject();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Main"))
				{
					RSM_Main();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Spotlight"))
				{
					RSM_Spot();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Point light"))
				{
					RSM_Point();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Cube"))
				{
					RSM_Cube();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}
	void GUIInspector::RSM_SelectedObject()
	{
		if (Select::isObjectSelected
			&& Select::selectedObj.IsInitialized())
		{
			GameObject obj = Select::selectedObj;

			string objID = "ID: " + to_string(obj.GetID());
			ImGui::Text(objID.c_str());

			ImGui::SameLine();

			Type objType = obj.GetMesh().GetType();
			string objTypeValue = "     Type: " + string(magic_enum::enum_name(objType));
			ImGui::Text(objTypeValue.c_str());

			ImGui::Spacing();

			ImGui::Text("Name");
			string objName = obj.GetName();
			strcpy_s(inputTextBuffer_objName, bufferSize, objName.c_str());
			if (ImGui::InputText("##objName", inputTextBuffer_objName, bufferSize))
			{
				obj.SetName(inputTextBuffer_objName);
			}

			ImGui::Separator();

			ImGui::Text("Transform");

			ImGui::Spacing();

			vec3 pos = obj.GetTransform().GetPosition();
			ImGui::Text("Position");
			if (ImGui::InputFloat3("##objPos", value_ptr(pos)))
			{
				obj.GetTransform().SetPosition(pos);

				vec3 pos = obj.GetTransform().GetPosition();
				string objPos = "( " +
					to_string(pos.x) + ", " +
					to_string(pos.y) + ", " +
					to_string(pos.z) + ")";
				string output = "moved " + obj.GetName() + " to " + objPos + "\n\n";

				cout << output << endl;
			}

			vec3 rot = obj.GetTransform().GetRotation();
			ImGui::Text("Rotation");
			if (ImGui::InputFloat3("##objRot", value_ptr(rot)))
			{
				obj.GetTransform().SetRotation(rot);
			}

			vec3 scale = obj.GetTransform().GetScale();
			ImGui::Text("Scale");
			if (ImGui::InputFloat3("##objScale", value_ptr(scale)))
			{
				obj.GetTransform().SetScale(scale);
			}
		}
	}
	void GUIInspector::RSM_Main()
	{
		ImGui::Text("Background color");
		ImGui::ColorEdit3("##bgrdiff", value_ptr(Render::backgroundColor));

		ImGui::Text("Directional light direction");
		ImGui::Text("x  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::SliderFloat("##dirX", &Render::directionalDirection.x, -360.0f, 360.0f);
		ImGui::SameLine();
		ImGui::Text("  y  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::SliderFloat("##dirY", &Render::directionalDirection.y, -360.0f, 360.0f);
		ImGui::SameLine();
		ImGui::Text("  z  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::SliderFloat("##dirZ", &Render::directionalDirection.z, -360.0f, 360.0f);

		ImGui::Text("Directional light diffuse");
		ImGui::ColorEdit3("##dirdiff", value_ptr(Render::directionalDiffuse));

		ImGui::Text("Directional light intensity");
		ImGui::SliderFloat("##dirint", &Render::directionalIntensity, 0.0f, 25.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how strong the directional light intensity is.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##dirint"))
		{
			Render::directionalIntensity = 1.0f;
		}

		ImGui::Text("Shininess");
		ImGui::SliderFloat("##shininess", &Render::shininess, 3.0f, 128.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how intense the material shininess should be.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##shininess"))
		{
			Render::shininess = 32.0f;
		}
	}
	void GUIInspector::RSM_Spot()
	{
		ImGui::Text("Spotlight diffuse");
		ImGui::ColorEdit3("##spotdiff", value_ptr(Render::spotDiffuse));

		ImGui::Text("Spotlight intensity");
		ImGui::SliderFloat("##spotint", &Render::spotIntensity, 0.0f, 25.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how strong the spotlight intensity is.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##spotint"))
		{
			Render::spotIntensity = 1.0f;
		}

		ImGui::Text("Spotlight distance");
		ImGui::SliderFloat("##spotdist", &Render::spotDistance, 0.0f, 25.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how far the spotlight can cast.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##spotdist"))
		{
			Render::spotDistance = 1.0f;
		}

		ImGui::Text("Spotlight inner angle");
		ImGui::SliderFloat("##spotinnerangle", &Render::spotInnerAngle, 0.0f, Render::spotOuterAngle - 0.01f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##spotinnerangle"))
		{
			Render::spotInnerAngle = 12.5f;
		}
		ImGui::Text("Spotlight outer angle");
		ImGui::SliderFloat("##spotouterangle", &Render::spotOuterAngle, Render::spotInnerAngle + 0.01f, 50.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##spotouterangle"))
		{
			Render::spotOuterAngle = 17.5f;
		}
	}
	void GUIInspector::RSM_Point()
	{
		ImGui::Text("Point light diffuse");
		ImGui::ColorEdit3("##pointdiff", value_ptr(Render::pointDiffuse));

		ImGui::Text("Point light intensity");
		ImGui::SliderFloat("##pointint", &Render::pointIntensity, 0.0f, 25.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how strong the point light intensity is.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##pointint"))
		{
			Render::pointIntensity = 1.0f;
		}

		ImGui::Text("Point light distance");
		ImGui::SliderFloat("##pointdist", &Render::pointDistance, 0.0f, 25.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how far the point light can cast.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##pointdist"))
		{
			Render::pointDistance = 1.0f;
		}
	}
	void GUIInspector::RSM_Cube()
	{
		ImGui::Text("Cube speed multiplier");
		ImGui::SliderFloat("##cubespeedmult", &Render::cubeSpeedMultiplier, 0.0f, 10.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust the overall cube speed multiplier.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##cubespeedmult"))
		{
			Render::cubeSpeedMultiplier = 0.005f;
		}

		ImGui::Text("Lamp orbit range");
		ImGui::SliderFloat("##lamporbitrange", &Render::lampOrbitRange, 2.0f, 10.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how far the cube should orbit from the lamp.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##lamporbitrange"))
		{
			Render::lampOrbitRange = 5.0f;
		}

		ImGui::Text("Cube wiggle height");
		ImGui::SliderFloat("##cubewheight", &Render::cubeWiggleHeight, 0.0f, 5.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how high and low the cube should wiggle.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##cubewheight"))
		{
			Render::cubeWiggleHeight = 2.0f;
		}

		ImGui::Text("Cube wiggle speed");
		ImGui::SliderFloat("##cubewspeed", &Render::cubeWiggleSpeed, 0.0f, 10.0f);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Adjust how fast the cube should wiggle.");
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##cubewspeed"))
		{
			Render::cubeWiggleSpeed = 1.0f;
		}
	}
}