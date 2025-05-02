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

namespace KalaKit::Physics::Core 
{
	class RigidBody;
}

namespace KalaKit::Physics::Shape 
{
	class BoxCollider;
	class SphereCollider;
}

//physics
#include "collision/manifold.hpp"

namespace KalaKit::Physics::Collision
{
	using KalaKit::Physics::Core::RigidBody;
	using KalaKit::Physics::Shape::BoxCollider;
	using KalaKit::Physics::Shape::SphereCollider;
	using KalaKit::Physics::Collision::SATResult;
	using KalaKit::Physics::Collision::ContactManifold;

	class KALAPHYSICS_API ContactGenerator
	{
	public:
		static ContactManifold GenerateBoxContacts(
			const RigidBody& bodyA,
			const RigidBody& bodyB,
			const BoxCollider& boxA,
			const BoxCollider& boxB,
			const SATResult& sat);

		static ContactManifold GenerateSphereContacts(
			const RigidBody& bodyA,
			const RigidBody& bodyB,
			const SphereCollider& sphereA,
			const SphereCollider& sphereB,
			const SATResult& sat);

		static ContactManifold GenerateBoxSphereContacts(
			const RigidBody& bodyA,
			const RigidBody& bodyB,
			const BoxCollider& boxA,
			const SphereCollider& sphereB);
	};
}