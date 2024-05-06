//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "glm.hpp"

//engine
#include "collision.hpp"

using glm::min;
using glm::max;
using glm::vec3;

using Graphics::Shape::Transform;

namespace Physics
{
	bool Collision::IsRayIntersectingCube(const Select::Ray& ray, const shared_ptr<GameObject>& cube)
	{
		//calculate the half extents of the cube along each axis
		vec3 pos = cube->GetTransform()->GetPosition();
		vec3 scale = cube->GetTransform()->GetScale();
		vec3 minBound = pos - 0.5f * scale;
		vec3 maxBound = pos + 0.5f * scale;

		//calculate the intersection distances along each axis
		vec3 tMin = (minBound - ray.origin) / ray.direction;
		vec3 tMax = (maxBound - ray.origin) / ray.direction;

		//find the intersection distances along each axis
		float tEnter = max(max(min(tMin.x, tMax.x), min(tMin.y, tMax.y)), min(tMin.z, tMax.z));
		float tExit = min(min(max(tMin.x, tMax.x), max(tMin.y, tMax.y)), max(tMin.z, tMax.z));

		//check if the ray intersects with the cube
		return tEnter < tExit && tExit > 0.0f;
	}
}