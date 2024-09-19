//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "imgui.h"

//engine
#include "selectobject.hpp"
#include "render.hpp"

using glm::inverse;
using glm::normalize;
using glm::max;
using glm::min;
using glm::vec4;
using std::numeric_limits;

using Graphics::Render;
using Type = Graphics::Shape::Mesh::MeshType;

namespace Core
{
	Select::Ray Select::RayFromMouse(float width, float height, double mouseX, double mouseY, const mat4& viewMatrix, const mat4& projectionMatrix)
	{
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
		float maxRange = 1000.0f;
		float closestDistance = maxRange;
		int closestIndex = -1;

		for (int i = 0; i < objects.size(); i++)
		{
			Type objType = objects[i]->GetMesh()->GetMeshType();
			if (objType == Type::model
				|| objType == Type::point_light
				|| objType == Type::spot_light
				|| objType == Type::directional_light)
			{
				float distance;
				if (IsRayIntersectingShape(ray, objects[i], &distance))
				{
					if (distance < closestDistance
						&& distance <= maxRange)
					{
						closestDistance = distance;
						closestIndex = i;
					}
				}
			}
		}

		return closestIndex;
	}

	bool Select::IsRayIntersectingShape(
		const Ray& ray, 
		const shared_ptr<GameObject>& shape,
		float* distance)
	{
		Type objType = shape->GetMesh()->GetMeshType();

		if (objType == Type::model 
			|| objType == Type::point_light 
			|| objType == Type::spot_light 
			|| objType == Type::directional_light)
		{
			vec3 pos = shape->GetTransform()->GetPosition();
			vec3 scale = shape->GetTransform()->GetScale();
			vec3 minBound = pos - 0.5f * scale;
			vec3 maxBound = pos + 0.5f * scale;

			vec3 tMin = (minBound - ray.origin) / ray.direction;
			vec3 tMax = (maxBound - ray.origin) / ray.direction;

			float tEnter = max(max(
				min(tMin.x, tMax.x), 
				min(tMin.y, tMax.y)), 
				min(tMin.z, tMax.z));
			float tExit = min(min(
				max(tMin.x, tMax.x), 
				max(tMin.y, tMax.y)), 
				max(tMin.z, tMax.z));

			if (tEnter < tExit && tExit > 0.0f)
			{
				*distance = tEnter;
				return true;
			}
		}

		return false;
	}
}