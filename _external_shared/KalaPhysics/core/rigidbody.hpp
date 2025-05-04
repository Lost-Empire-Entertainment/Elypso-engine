//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//external
#include "glm.hpp"
#include "gtc/quaternion.hpp"

//physics
#include "core/utils.hpp"
#include "core/gameobjecthandle.hpp"
#include "collision/collider.hpp"

namespace KalaKit::Physics::Core
{
	using glm::vec3;
	using glm::quat;

	using KalaKit::Physics::Collision::Collider;
	using KalaKit::Physics::Collision::ColliderType;

	class KALAPHYSICS_API RigidBody
	{
	public:
		GameObjectHandle handle;        //Reference to the associated game object
		vec3 position;                  //This rigidbody position
		quat rotation;                  //This rigidbody rotation
		vec3 scale;                     //This rigidbody scale
		vec3 velocity = vec3(0);        //Linear velocity
		vec3 angularVelocity = vec3(0); //Angular velocity (rotation speed)
		float mass;                     //Object's mass affecting inertia
		bool isDynamic;                 //True if the object responds to forces
		Collider* collider = nullptr;   //Pointer to the object's collider
		vec3 inertiaTensor = vec3(0);   //Precomputed inertia tensor for rotations

		float restitution;              //Bounciness factor after collisions
		float staticFriction;           //Resists initial movement when at rest
		float dynamicFriction;          //Slows down sliding objects
		float gravityFactor;            //Gravity multiplier (1.0 = normal gravity)
		bool useGravity;                //True if affected by global gravity

		float tiltTimer = 0.0f;         //Tracks time elapsed since the last tilt correction for this body
		float angleToFlat = 0.0f;       //Tilt angle from the best UP-facing side
		vec3 closestUp = vec3(0, 1, 0); //Best face to align with UP
		vec3 centerOfGravity = vec3(0); //Offset from the combined position

		bool isSleeping = false;        //True if the object is inactive
		float sleepThreshold = 0.01f;   //Velocity below this puts object to sleep
		float sleepTimer = 0.0f;        //Time spent inactive before sleeping

		RigidBody(
			GameObjectHandle h,
			const vec3& position,
			const quat& rotation,
			const vec3& scale,
			bool isDynamic = false,
			bool useGravity = false,
			float m = 1.0f,
			float rest = 0.0f,
			float staticFrict = 0.9f,
			float dynamicFrict = 0.7f,
			float gFactor = 1.0f);

		/// <summary>
		/// Apply linear force
		/// </summary>
		void ApplyForce(const vec3& force);
		/// <summary>
		/// Apply an instant impulse (collision effects)
		/// </summary>
		void ApplyImpulse(const vec3& impulse);
		/// <summary>
		/// Apply rotational force
		/// </summary>
		void ApplyTorque(const vec3& torque);
		/// <summary>
		/// Precompute inertia tensor based off of collider scale
		/// </summary>
		void ComputeInertiaTensor();
		/// <summary>
		/// Dynamically updates center of gravity for physics calculations
		/// </summary>
		void UpdateCenterOfGravity();
		/// <summary>
		/// Assign a new scale to this rigidbody. This affects the center of gravity and inertia tensor.
		/// </summary>
		void SetScale(const vec3& scale);

		/// <summary>
		/// Assign a new collider or change the existing collider of the rigidbody
		/// </summary>
		void SetCollider(ColliderType type);

		/// <summary>
		/// Wake up the body
		/// </summary>
		void WakeUp();
		/// <summary>
		/// Put the body to sleep
		/// </summary>
		void Sleep();
	};
}