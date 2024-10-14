//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <memory>

//external
#include "glm.hpp"
#include "gameobject.hpp"

namespace Core
{
	using std::vector;
	using glm::vec3;
	using glm::mat4;
	using std::shared_ptr;

	using Graphics::Shape::AssimpVertex;
	using Graphics::Shape::GameObject;

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
		/// <param name="width">Target imgui window width</param>
		/// <param name="height">Target imgui window height</param>
		/// <param name="mouseX">Mouse X position</param>
		/// <param name="mouseY">Mouse Y position</param>
		/// <param name="viewMatrix">Target camera view matrix</param>
		/// <param name="projectionMatrix">Target camera projection matrix</param>
		/// <returns></returns>
		static Ray RayFromMouse(
			float width, 
			float height, 
			double mouseX, 
			double mouseY, 
			const mat4& viewMatrix, 
			const mat4& projectionMatrix);

		/// <summary>
		/// Check ray-object intersections.
		/// </summary>
		static int CheckRayObjectIntersections(
			const Ray& ray, 
			const vector<shared_ptr<GameObject>>& objects);

		/// <summary>
		/// Calculates the bounding box (interaction box) based on the provided vertices, 
		/// adjusting it with the object's position, scale, and an additional margin.
		/// </summary>
		/// <param name="vertices">A list of vertices from the object.</param>
		/// <param name="minBound">The calculated minimum bound of the interaction box (output).</param>
		/// <param name="maxBound">The calculated maximum bound of the interaction box (output).</param>
		/// <param name="position">The position of the object in world space.</param>
		/// <param name="scale">The scale of the object to apply to the vertices.</param>
		/// <param name="margin">An additional margin to expand the interaction box beyond the object’s bounds.</param>
		static void CalculateInteractionBoxFromVertices(
			const vector<AssimpVertex>& vertices,
			vec3& minBound,
			vec3& maxBound,
			const vec3& position,
			const vec3& scale,
			float margin);
	private:
		/// <summary>
		/// Check if the ray actually is interacting with a shape
		/// </summary>
		static bool IsRayIntersectingShape(
			const Ray& ray, 
			const shared_ptr<GameObject>& cube,
			float* distance);
	};
}