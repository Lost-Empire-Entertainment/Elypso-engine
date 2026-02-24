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

	using f32 = float;

	inline const vec3 MIN_BCP_POS = vec3(-10000.0f);
	inline const vec3 MAX_BCP_POS = vec3(10000.0f);

	constexpr f32 MIN_BCP_HEIGHT = 1e-6f;
	constexpr f32 MAX_BCP_HEIGHT = 10000.0f;

	constexpr f32 MIN_BCP_RADIUS = 1e-6f;
	constexpr f32 MAX_BCP_RADIUS = 10000.0f;

	class LIB_API Collider_BCP : public Collider
	{
		friend class KalaPhysics::Core::PhysicsWorld;
	public:
		//Initializes a broadphase or narrowphase BCP collider
		static Collider_BCP* Initialize(
			u32 parentRigidBody,
			const vec3& pos,
			f32 height,
			f32 radius,
			ColliderType type);

		const vec3& GetPos() const;
		void SetPos(const vec3& newValue);

		f32 GetHeight() const;
		void SetHeight(f32 newValue);

		f32 GetRadius() const;
		void SetRadius(f32 newValue);

		~Collider_BCP() override;
	private:
		void Update(Collider* c, f32 deltaTime) override;

		vec3 pos{};
		f32 height{};
		f32 radius{};
	};
}