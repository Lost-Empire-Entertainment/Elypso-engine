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

//external
#include "glm.hpp"

//physics
#include "core/gameobjecthandle.hpp"
#include "shape/collider.hpp"
#include "collision/manifold.hpp"

namespace KalaKit::Physics::Shape
{
	using glm::vec3;

	using KalaKit::Physics::Core::GameObjectHandle;
	using KalaKit::Physics::Collision::ContactManifold;

	class KALAPHYSICS_API SphereCollider : public Collider
	{
	public:
		float radius = 0.0f;

		SphereCollider(const GameObjectHandle& handle);

		void CalculateBoundingRadius() override;

		ContactManifold GenerateContacts(
			const RigidBody& self,
			const RigidBody& other,
			const Collider& otherCol) const override;
	};
}