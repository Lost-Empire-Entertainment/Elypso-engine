//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//external
#include "physicsworld.hpp"
#include "glm.hpp"

namespace Core
{
	using KalaKit::PhysicsWorld;
	using glm::vec3;

	class Physics
	{
	public:
		static inline bool simulatePhysics;

		static inline PhysicsWorld* physicsWorld = nullptr;

		static void Initialize(const vec3& gravity);
		static void Update(float deltaTime);
	};
}