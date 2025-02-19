//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//engine
#include "physics.hpp"
#include "console.hpp"

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
	PhysicsWorld& Physics::GetPhysicsWorld()
	{
		if (!physicsWorld) physicsWorld = &PhysicsWorld::GetInstance();
		return *physicsWorld;
	}

	void Physics::Initialize(const vec3& gravity)
	{
		if (!physicsWorld) GetPhysicsWorld();
		physicsWorld->InitializePhysics(gravity);

		ConsoleManager::WriteConsoleMessage(
			Caller::INITIALIZE,
			Type::INFO,
			"Successfully initialized Elypso Physics!\n");
	}

	void Physics::Update(float deltaTime)
	{
		physicsWorld->StepSimulation(deltaTime);
	}

	void Physics::Shutdown()
	{
		physicsWorld->ShutdownPhysics();
	}
}