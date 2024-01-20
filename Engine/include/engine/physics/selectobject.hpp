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

#pragma once

//external
#include "glm.hpp"
#include "gameobject.hpp"

#include <vector>

using std::vector;
using glm::vec3;
using glm::mat4;

using Graphics::Shape::GameObject;

namespace Physics
{
	class Select
	{
	public:
		struct Ray
		{
			vec3 origin;
			vec3 direction;
		};

		static inline bool isObjectSelected;
		static inline shared_ptr<GameObject> selectedObj;

		/// <summary>
		/// Calculate a ray from mouse coordinates.
		/// </summary>
		static Ray RayFromMouse(double mouseX, double mouseY, const mat4& viewMatrix, const mat4& projectionMatrix);

		/// <summary>
		/// Check ray-object intersections.
		/// </summary>
		static int CheckRayObjectIntersections(const Ray& ray, const vector<shared_ptr<GameObject>>& objects);
	};
}