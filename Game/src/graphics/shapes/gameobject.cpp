//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <algorithm>

//external
#include "glm.hpp"

//game
#include "gameobject.hpp"
#include "model.hpp"
#include "modelchild.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "billboard.hpp"
#include "render.hpp"

using std::cout;
using std::endl;
using std::sort;
using std::to_string;
using std::remove;
using std::dynamic_pointer_cast;
using glm::distance;

using Type = Graphics::Shape::Mesh::MeshType;
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
				if (obj->GetName() == "") obj->SetName(".");

				Type type = obj->GetMesh()->GetMeshType();
				switch (type)
				{
				case Type::model:
					ModelChild::Render(obj, view, projection);
					break;
				case Type::point_light:
					PointLight::RenderPointLight(obj, view, projection);
					break;
				case Type::spot_light:
					SpotLight::RenderSpotLight(obj, view, projection);
					break;
				}
			}
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

			for (const auto& obj : transparentObjects)
			{
				if (obj->GetName() == "") obj->SetName(".");

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

		//destroy all children if parent is destroyed
		if (obj->GetChildren().size() > 0)
		{
			for (const auto& child : obj->GetChildren())
			{
				GameObjectManager::DestroyGameObject(child);
			}
		}
		//remove object from parent children vector
		if (obj->GetParent() != nullptr)
		{
			obj->GetParent()->RemoveChild(obj);
		}

		switch (type)
		{
		case Type::model:
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