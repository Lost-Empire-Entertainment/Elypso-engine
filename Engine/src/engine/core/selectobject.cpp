//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "imgui.h"

//engine
#include "selectobject.hpp"
#include "render.hpp"
#include "gameobject.hpp"
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
			//get vertices from the mesh
			const vector<AssimpVertex>& vertices = shape->GetMesh()->GetVertices();

			//calculate the bounding box using vertices and object's position and scale
			vec3 minBound, maxBound;
			vec3 position = shape->GetTransform()->GetPosition();
			vec3 scale = shape->GetTransform()->GetScale(); //apply scale transformation
			float margin = 0.1f; //adjust margin as necessary

			CalculateInteractionBoxFromVertices(vertices, minBound, maxBound, position, scale, margin);

			//perform ray-box intersection with the updated bounding box
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

	void Select::CalculateInteractionBoxFromVertices(
		const vector<AssimpVertex>& vertices,
		vec3& minBound,
		vec3& maxBound,
		const vec3& position,
		const vec3& scale,
		float margin)
	{
		if (vertices.empty()) 
		{
			minBound = vec3(0.0f);
			maxBound = vec3(0.0f);
			return;
		}

		//initialize the bounds using the first vertex position
		minBound = position + vertices[0].pos * scale;
		maxBound = position + vertices[0].pos * scale;

		//iterate over all vertices to find min and max bounds
		for (const auto& vertex : vertices)
		{
			vec3 transformedPos = position + vertex.pos * scale;

			minBound.x = std::min(minBound.x, transformedPos.x);
			minBound.y = std::min(minBound.y, transformedPos.y);
			minBound.z = std::min(minBound.z, transformedPos.z);

			maxBound.x = std::max(maxBound.x, transformedPos.x);
			maxBound.y = std::max(maxBound.y, transformedPos.y);
			maxBound.z = std::max(maxBound.z, transformedPos.z);
		}

		//apply margin to make the bounding box slightly larger
		vec3 marginVec(margin, margin, margin);
		minBound -= marginVec;
		maxBound += marginVec;
	}
}