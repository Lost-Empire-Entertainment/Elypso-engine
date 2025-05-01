//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "physics.hpp"

namespace Core
{
	void Physics::Initialize(const vec3& gravity)
	{
		if (!physicsWorld) physicsWorld = &PhysicsWorld::GetInstance();
		physicsWorld->InitializePhysics(gravity);

	}
	void Physics::Update(float deltaTime)
	{
		physicsWorld->StepSimulation(deltaTime);
	}
}