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

	inline const vec3 MIN_KDOP_POS = vec3(-10000.0f);
	inline const vec3 MAX_KDOP_POS = vec3(10000.0f);

	enum class KDOPShape : u8
	{
		KDOP_10_X = 0, //10-face Z-axis discrete oriented polytope
		KDOP_10_Y = 1, //10-face Y-axis discrete oriented polytope
		KDOP_10_Z = 2, //10-face Z-axis discrete oriented polytope
		KDOP_18 = 3,   //18-face discrete oriented polytope
		KDOP_26 = 4    //26-face discrete oriented polytope
	};

	class LIB_API Collider_KDOP : public Collider
	{
		friend class KalaPhysics::Core::PhysicsWorld;
	public:
		//Initializes a broadphase-only KDOP collider
		static Collider_KDOP* Initialize(
			u32 parentRigidBody,
			const vec3& pos,
			const quat& rot,
			const vector<vec3>& vertices,
			KDOPShape shape);

		const vec3& GetPos() const;
		void SetPos(const vec3& newValue);

		const quat& GetRot() const;
		void SetRot(const quat& newValue);

		const vector<vec3>& GetVertices() const;

		KDOPShape GetKDOPShape() const;

		~Collider_KDOP() override;
	private:
		void Update(Collider* c, f32 deltaTime) override;

		vec3 pos{};
		quat rot{};

		vector<vec3> vertices{};

		KDOPShape kdopShape{};
	};
}