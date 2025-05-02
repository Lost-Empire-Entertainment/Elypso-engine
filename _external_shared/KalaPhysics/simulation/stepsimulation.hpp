//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAPHYSICS_DLL_EXPORT
		#define KALAPHYSICS_API __declspec(dllexport)
	#else
		#define KALAPHYSICS_API __declspec(dllimport)
	#endif
#else
	#define KALAPHYSICS_API
#endif

#include "glm.hpp"

//physics
#include "core/rigidbody.hpp"
#include "core/physicsworld.hpp"

namespace KalaKit::Physics::Simulation
{
	using glm::vec3;

	using KalaKit::Physics::Core::RigidBody;
	using KalaKit::Physics::Core::PhysicsWorld;

	class KALAPHYSICS_API StepSimulation
	{
	public:
		/// <summary>
		/// Update physics simulation
		/// </summary>
		static void Step(PhysicsWorld& world, float deltaTime);
	private:
		static bool IsValidCollision(RigidBody& bodyA, RigidBody& bodyB);

		static void ApplyPhysicsIntegration(PhysicsWorld& world, float deltaTime);

		static void PredictCollision(
			PhysicsWorld& world,
			RigidBody* bodyPtr,
			RigidBody& body,
			float deltaTime);

		static bool CanTilt(RigidBody& body);
		static void TiltBody(RigidBody& body);
	};
}