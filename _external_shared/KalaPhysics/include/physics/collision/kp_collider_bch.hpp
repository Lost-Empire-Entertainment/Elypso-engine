//Copyright(C) 2026 Lost Empire Entertainment
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
	using KalaHeaders::KalaMath::normalize_q;

	inline const vec3 MIN_BCH_POS = vec3(-10000.0f);
	inline const vec3 MAX_BCH_POS = vec3(10000.0f);

	class LIB_API Collider_BCH : public Collider
	{
		friend class KalaPhysics::Core::PhysicsWorld;
	public:
		//Initializes a narrowphase-only BCH collider
		static Collider_BCH* Initialize(
			u32 parentRigidBody,
			const vec3& pos,
			const quat& rot,
			const vector<vec3>& vertices);

		const vec3& GetPos() const;
		void SetPos(const vec3& newValue);

		const quat& GetRot() const;
		void SetRot(const quat& newValue);

		const vector<vec3>& GetVertices() const;

		~Collider_BCH() override;
	private:
		void Update(Collider* c, f32 deltaTime) override;

		vec3 pos{};
		quat rot{};

		vector<vec3> vertices{};
	};
}