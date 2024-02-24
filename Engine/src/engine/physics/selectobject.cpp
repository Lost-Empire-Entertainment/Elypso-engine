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

//engine
#include "selectobject.hpp"
#include "render.hpp"
#include "collision.hpp"

using glm::inverse;
using glm::normalize;
using glm::vec4;
using std::numeric_limits;

using Graphics::Render;
using Physics::Collision;
using Type = Graphics::Shape::Mesh::MeshType;

namespace Physics
{
	Select::Ray Select::RayFromMouse(double mouseX, double mouseY, const mat4& viewMatrix, const mat4& projectionMatrix)
	{
		int width, height;
		glfwGetWindowSize(Render::window, &width, &height);
		float x = (2.0f * static_cast<float>(mouseX)) / width - 1.0f;
		float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / height;

		vec4 rayClip = vec4(x, y, -1.0, 1.0);
		vec4 rayEye = inverse(projectionMatrix) * rayClip;
		rayEye = vec4(rayEye.x, rayEye.y, -1.0, 0.0);

		vec4 rayWorld = inverse(viewMatrix) * rayEye;
		vec3 rayDirection = normalize(vec3(rayWorld));

		return Ray(Render::camera.GetCameraPosition(), rayDirection);
	}

	int Select::CheckRayObjectIntersections(const Ray& ray, const vector<shared_ptr<GameObject>>& objects)
	{
		//if user pressed left mouse button over any imgui window
		if (ImGui::GetIO().WantCaptureMouse)
		{
			return -2;
		}
		else
		{
			for (int i = 0; i < objects.size(); i++)
			{
				Type objType = objects[i]->GetMesh()->GetMeshType();
				if ((objType == Type::cube
					|| objType == Type::point_light
					|| objType == Type::spot_light)
					&& Collision::IsRayIntersectingCube(ray, objects[i]))
				{
					return i;
				}
			}

			//if user did not press any valid gameobject
			return -1;
		}
	}
}