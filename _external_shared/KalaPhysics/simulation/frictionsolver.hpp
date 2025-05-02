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

namespace KalaKit::Physics::Simulation
{
	struct Contact;
}

#include <vector>

//external
#include "glm.hpp"

//physics
#include "simulation/solver.hpp"
#include "core/rigidbody.hpp"

namespace KalaKit::Physics::Simulation
{
	using glm::vec3;
	using std::vector;

	using KalaKit::Physics::Core::RigidBody;

	class KALAPHYSICS_API FrictionSolver : public Solver
	{
	public:
		void AddFrictionPair(
			RigidBody* bodyA,
			RigidBody* bodyB,
			const vec3& point,
			const vec3& normal,
			float staticFriction,
			float dynamicFriction,
			Contact* linkedContact);

		void Solve(float deltaTime, int iterations) override;
		void Clear() override;
	private:
		struct FrictionConstraint
		{
			RigidBody* bodyA;
			RigidBody* bodyB;
			vec3 point;
			vec3 normal;
			vec3 tangent;
			vec3 rA;
			vec3 rB;
			float effectiveMass;
			float tangentImpulse;
			float accumulatedImpulse;
			Contact* linkedContact;
		};

		vector<FrictionConstraint> constraints;
	};
}