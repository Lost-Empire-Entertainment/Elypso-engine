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

//physics
#include "core/physicsworld.hpp"
#include "core/rigidbody.hpp"

namespace KalaKit::Physics::Simulation
{
	using KalaKit::Physics::Core::PhysicsWorld;
	using KalaKit::Physics::Core::RigidBody;

	class KALAPHYSICS_API HandleMotion
	{
	public:
		static void Run(
			PhysicsWorld& world, 
			float deltaTime);
	private:
		static void ApplyForces(
			PhysicsWorld& world,
			RigidBody& body,
			float deltaTime);

		static void ProjectAgainstSurfaces(
			PhysicsWorld& world,
			RigidBody* bodyPtr,
			RigidBody& body,
			float deltaTime);

		static void Integrate(
			PhysicsWorld& world,
			RigidBody& body,
			float deltaTime);

		static void ApplyDamping(
			RigidBody& body,
			float deltaTime);

		static void UpdateSleepState(
			RigidBody& body,
			float deltaTime);

		static bool CanTilt(RigidBody& body);
		static void TiltBody(RigidBody& body);
	};
}