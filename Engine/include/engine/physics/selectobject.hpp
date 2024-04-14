//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

//external
#include "glm.hpp"
#include "gameobject.hpp"

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