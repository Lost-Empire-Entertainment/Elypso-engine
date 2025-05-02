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

#include <vector>

//external
#include "glm.hpp"

//physics
#include "simulation/solver.hpp"
#include "core/rigidbody.hpp"
#include "core/physicsworld.hpp"

namespace KalaKit::Physics::Simulation
{
	using glm::vec3;
	using std::vector;

	using KalaKit::Physics::Core::RigidBody;
	using KalaKit::Physics::Core::PhysicsWorld;

	struct Contact
	{
		RigidBody* bodyA;
		RigidBody* bodyB;
		vec3 point;
		vec3 normal;
		vec3 rA;
		vec3 rB;
		float penetration;
		float effectiveMass;
		float accumulatedImpulse;
		float bias;
	};

	class KALAPHYSICS_API ContactSolver : public Solver
	{
	public:
		Contact& AddContact(
			PhysicsWorld& world,
			RigidBody* bodyA,
			RigidBody* bodyB,
			const vec3& point,
			const vec3& normal,
			float penetration,
			float deltaTime);

		void Solve(float deltaTime, int interations) override;
		void Clear() override;
	private:
		vector<Contact> contacts;
	};
}