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
	using ElypsoPhysics::PhysicsWorld;
	using glm::vec3;

	class Physics
	{
	public:
		static PhysicsWorld& GetPhysicsWorld();

		static void Initialize(const vec3& gravity = vec3(0.0f, 9.81f, 0.0f));

		static void Update(float deltaTime);

		static void Shutdown();
	private:
		static inline PhysicsWorld* physicsWorld = nullptr;
	};
}