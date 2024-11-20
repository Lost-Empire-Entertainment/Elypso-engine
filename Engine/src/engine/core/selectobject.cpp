//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <algorithm>

//external
#include "imgui.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"
#include "glm.hpp"

//engine
#include "selectobject.hpp"
#include "render.hpp"
#include "gameobject.hpp"

using glm::inverse;
using glm::normalize;
using glm::vec4;
using std::numeric_limits;
using glm::quat;
using glm::scale;
using std::ranges::max;
using std::ranges::min;

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
			vec3 minBound, maxBound;
			vec3 pos = shape->GetTransform()->GetPosition();
			vec3 objectScale = shape->GetTransform()->GetScale();
			quat rotation = quat(radians(shape->GetTransform()->GetRotation()));

			if (objType == Type::model)
			{
				const vector<AssimpVertex>& vertices = shape->GetMesh()->GetVertices();

				//complex bounding box for models
				CalculateInteractionBoxFromVertices(vertices, minBound, maxBound, vec3(0.0f), vec3(1.0f));
			}
			else
			{
				//simple bounding box for non-model objects
				minBound = -0.5f * objectScale;
				maxBound = 0.5f * objectScale;
			}

			//create the model matrix (translation, rotation, scale)
			mat4 modelMatrix = 
				translate(mat4(1.0f), pos) 
				* mat4_cast(rotation) 
				* scale(mat4(1.0f), objectScale);

			//inverse the model matrix to transform the ray into local space
			mat4 inverseModel = inverse(modelMatrix);

			//transform ray to local space
			vec3 localRayOrigin = vec3(inverseModel * vec4(ray.origin, 1.0f));
			vec3 localRayDir = normalize(vec3(inverseModel * vec4(ray.direction, 0.0f)));

			//ensure the direction is valid
			if (length(localRayDir) == 0.0f) return false;

			//perform ray-box intersection in local space
			vec3 tMin = (minBound - localRayOrigin) / localRayDir;
			vec3 tMax = (maxBound - localRayOrigin) / localRayDir;

			float tEnter = max({ min(tMin.x, tMax.x),
								 min(tMin.y, tMax.y),
								 min(tMin.z, tMax.z) });
			float tExit = min({ max(tMin.x, tMax.x),
								max(tMin.y, tMax.y),
								max(tMin.z, tMax.z) });

			if (tEnter > tExit
				|| tExit < 0.0f)
			{
				return false;
			}

			//calculate intersection point in local space
			vec3 intersectionLocal = localRayOrigin + tEnter * localRayDir;

			//transform intersection point back to world space
			vec3 intersectionWorld = vec3(modelMatrix * vec4(intersectionLocal, 1.0f));

			//compute distance from ray origin to intersection point in world space
			*distance = length(intersectionWorld - ray.origin);

			return true;
		}

		return false;
	}

	void Select::CalculateInteractionBoxFromVertices(
		const vector<AssimpVertex>& vertices,
		vec3& minBound,
		vec3& maxBound,
		const vec3& position,
		const vec3& scale)
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

			minBound.x = min(minBound.x, transformedPos.x);
			minBound.y = min(minBound.y, transformedPos.y);
			minBound.z = min(minBound.z, transformedPos.z);

			maxBound.x = max(maxBound.x, transformedPos.x);
			maxBound.y = max(maxBound.y, transformedPos.y);
			maxBound.z = max(maxBound.z, transformedPos.z);
		}
	}
}