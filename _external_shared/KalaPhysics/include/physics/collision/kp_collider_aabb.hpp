//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/core_utils.hpp"

#include "physics/collision/kp_collider.hpp"

namespace KalaPhysics::Core
{
	class PhysicsWorld;
}

namespace KalaPhysics::Physics::Collision
{
	using KalaHeaders::KalaMath::kclamp;

	inline const vec3 MIN_AABB_CORNER = vec3(-10000.0f);
	inline const vec3 MAX_AABB_CORNER = vec3(10000.0f);
	inline const vec3 MIN_AABB_CORNER_DISTANCE = vec3(1e-6f);

	class LIB_API Collider_AABB : public Collider
	{
		friend class KalaPhysics::Core::PhysicsWorld;
	public:
		//Initializes a broadphase-only AABB collider
		static Collider_AABB* Initialize(
			u32 parentRigidBody,
			const vec3& minCorner,
			const vec3& maxCorner);

		const vec3& GetMinCorner() const;
		void SetMinCorner(const vec3& newValue);

		const vec3& GetMaxCorner() const;
		void SetMaxCorner(const vec3& newValue);

		~Collider_AABB() override;
	private:
		void Update(Collider* c, f32 deltaTime) override;

		vec3 minCorner{};
		vec3 maxCorner{};
	};
}