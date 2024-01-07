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
#include "glm.hpp"

//engine
#include "collision.hpp"

using glm::min;
using glm::max;
using glm::vec3;

using Core::ECS::Transform;

namespace Physics
{
	bool Collision::IsRayIntersectingCube(const Select::Ray& ray, const GameObject& cube)
	{
		//calculate the half extents of the cube along each axis
		vec3 pos = cube.GetComponent<Transform>()->position;
		vec3 scale = cube.GetComponent<Transform>()->scale;
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