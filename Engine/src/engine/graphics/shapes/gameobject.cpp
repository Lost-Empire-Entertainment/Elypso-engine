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
#include <algorithm>

//external
#include "glm.hpp"

//engine
#include "gameobject.hpp"
#include "cube.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "selectedobjectaction.hpp"
#include "selectedobjectborder.hpp"
#include "billboard.hpp"
#include "render.hpp"
#include "selectobject.hpp"

using std::cout;
using std::endl;
using std::sort;
using std::to_string;
using std::remove;
using std::dynamic_pointer_cast;
using glm::distance;

using Physics::Select;
using Type = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::ActionTex;
using Graphics::Shape::Border;
using Graphics::Render;

namespace Graphics::Shape
{
	void GameObjectManager::RenderAll(const mat4& view, const mat4& projection)
	{
		//opaque objects are rendered first
		if (opaqueObjects.size() > 0)
		{
			for (const auto& obj : opaqueObjects)
			{
				Type type = obj->GetMesh()->GetMeshType();
				switch (type)
				{
				case Type::cube:
					Cube::RenderCube(obj, view, projection);
					break;
				case Type::point_light:
					PointLight::RenderPointLight(obj, view, projection);
					break;
				case Type::spot_light:
					SpotLight::RenderSpotLight(obj, view, projection);
					break;
				}
			}

			Border::RenderBorder(border, view, projection);
		}

		//transparent objects are rendered last
		if (transparentObjects.size() > 0)
		{
			sort(transparentObjects.begin(), transparentObjects.end(),
				[&view](const auto& a, const auto& b)
				{
					//calculate the distance along the viewing direction vector from the camera
					vec3 cameraPosition = vec3(view[3]);
					vec3 objectPositionA = a->GetTransform()->GetPosition();
					vec3 objectPositionB = b->GetTransform()->GetPosition();

					//project object positions onto the viewing direction vector
					float distanceA = dot(objectPositionA - cameraPosition, vec3(view[2]));
					float distanceB = dot(objectPositionB - cameraPosition, vec3(view[2]));

					//sort based on the projected distances
					return distanceA > distanceB; //render from back to front
				});

			glDisable(GL_DEPTH_TEST);

			ActionTex::RenderActionTex(actionTex, view, projection);

			for (const auto& obj : transparentObjects)
			{
				Type type = obj->GetMesh()->GetMeshType();
				switch (type)
				{
				case Type::billboard:
					Billboard::RenderBillboard(obj, view, projection);
					break;
				}
			}

			glEnable(GL_DEPTH_TEST);
		}
	}

	void GameObjectManager::DestroyGameObject(const shared_ptr<GameObject>& obj)
	{
		Type type = obj->GetMesh()->GetMeshType();

		Select::selectedObj = nullptr;
		Select::isObjectSelected = false;

		switch (type)
		{
		case Type::cube:
			objects.erase(remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			break;
		case Type::point_light:
			DestroyGameObject(obj->GetChildBillboard());
			obj->SetChildBillboard(nullptr);
			objects.erase(remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			pointLights.erase(remove(pointLights.begin(), pointLights.end(), obj), pointLights.end());
			break;
		case Type::spot_light:
			DestroyGameObject(obj->GetChildBillboard());
			obj->SetChildBillboard(nullptr);
			objects.erase(remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			spotLights.erase(remove(spotLights.begin(), spotLights.end(), obj), spotLights.end());
			break;
		case Type::billboard:
			obj->SetParentBillboardHolder(nullptr);
			objects.erase(remove(objects.begin(), objects.end(), obj), objects.end());
			transparentObjects.erase(remove(transparentObjects.begin(), transparentObjects.end(), obj), transparentObjects.end());
			billboards.erase(remove(billboards.begin(), billboards.end(), obj), billboards.end());
			break;
		}
	}
}