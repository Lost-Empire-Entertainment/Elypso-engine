//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>

//external
#include "physicsworld.hpp"
#include "gameobjecthandle.hpp"
#include "collider.hpp"

//engine
#include "component.hpp"
#include "gameobject.hpp"

namespace Graphics::Components
{
	using ElypsoPhysics::PhysicsWorld;
	using ElypsoPhysics::GameObjectHandle;
	using ElypsoPhysics::ColliderType;
	using ElypsoPhysics::Collider;
	using Graphics::Shape::GameObject;

	using std::string;
	using std::shared_ptr;

	class RigidBodyComponent : public Component
	{
	public:
		RigidBodyComponent(
			shared_ptr<GameObject> owner,
			ColliderType colliderType = ColliderType::BOX,
			bool isDynamic = false,
			bool useGravity = false,
			float gravityFactor = 1.0f,
			float mass = 1.0f,
			float restitution = 0.0f,
			float staticFriction = 0.9f,
			float dynamicFriction = 0.7f);

		void SetHandle(const GameObjectHandle& newHandle) { handle = newHandle; }

		void SetOffsetPosition(const vec3& newPos) const;
		void SetCombinedPosition(const vec3& newPos) const;
		void SetOffsetRotation(const vec3& newRot) const;
		void SetCombinedRotation(const vec3& newRot) const;
		void SetOffsetScale(const vec3& newScale) const;
		void SetCombinedScale(const vec3& newScale) const;

		void ResetVelocity() const;
		void ResetAngularVelocity() const;

		/// <summary>
		/// Apply linear force
		/// </summary>
		void ApplyForce(const vec3& force) const;
		/// <summary>
		/// Apply an instant impulse (collision effects)
		/// </summary>
		void ApplyImpulse(const vec3& impulse) const;
		/// <summary>
		/// Apply rotational force
		/// </summary>
		void ApplyTorque(const vec3& torque) const;

		void SetColliderType(ColliderType newColliderType) const;
		void SetDynamic(bool newIsDynamic);
		void EnableGravity(bool newUseGravity);
		void SetGravityFactor(float newGravityFactor);
		void SetMass(float newMass);
		void SetRestitution(float newRestitution);
		void SetStaticFriction(float newStaticFriction);
		void SetDynamicFriction(float newDynamicFriction);

		const GameObjectHandle& GetHandle() const { return handle; }

		vec3 GetOffsetPosition() const;
		vec3 GetCombinedPosition() const;
		vec3 GetOffsetRotation() const;
		vec3 GetCombinedRotation() const;
		vec3 GetOffsetScale() const;
		vec3 GetCombinedScale() const;
		vec3 GetVelocity() const;
		vec3 GetAngularVelocity() const;

		vec3 GetCenterOfGravity() const;

		ColliderType GetColliderType() const;
		Collider* GetCollider() const;
		bool IsDynamic() const { return isDynamic; }
		bool UseGravity() const { return useGravity; }
		float GetGravityFactor() const { return gravityFactor; }
		float GetMass() const { return mass; }
		float GetRestitution() const { return restitution; }
		float GetStaticFriction() const { return staticFriction; }
		float GetDynamicFriction() const { return dynamicFriction; }

		type_index GetType() const override { return typeid(RigidBodyComponent); }

		void Update(float deltaTime) override {}

	private:
		GameObjectHandle handle;
		bool isDynamic;
		bool useGravity;
		float gravityFactor;
		float mass;
		float restitution;
		float staticFriction;
		float dynamicFriction;
	};
}