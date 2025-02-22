//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef PHYSICS_DLL_EXPORT
		#define PHYSICS_API __declspec(dllexport)
	#else
		#define PHYSICS_API __declspec(dllimport)
	#endif
#else
	#define PHYSICS_API
#endif

//external
#include "glm.hpp"
#include "gtc/quaternion.hpp"

//physics
#include "gameobjecthandle.hpp"
#include "collider.hpp"

namespace ElypsoPhysics
{
	using glm::vec3;
	using glm::quat;

	class PHYSICS_API RigidBody
	{
	public:
		GameObjectHandle handle;       //Reference to the associated game object
		vec3 offsetPosition;           //The offset position relative to the target gameobject
		vec3 combinedPosition;         //The combined position of target gameobject position and local offset
		quat offsetRotation;           //The offset rotation relative to the target gameobject
		quat combinedRotation;         //The combined rotation of target gameobject rotation and local offset
		vec3 velocity;                 //Linear velocity
		vec3 angularVelocity;          //Angular velocity (rotation speed)
		float mass;                    //Object's mass affecting inertia
		bool isDynamic;                //True if the object responds to forces
		Collider* collider;            //Pointer to the object's collider
		vec3 inertiaTensor;            //Precomputed inertia tensor for rotations

		float restitution;             //Bounciness factor after collisions
		float staticFriction;          //Resists initial movement when at rest
		float dynamicFriction;         //Slows down sliding objects
		float gravityFactor;           //Gravity multiplier (1.0 = normal gravity)
		bool useGravity;               //True if affected by global gravity

		bool isSleeping = false;       //True if the object is inactive
		float sleepThreshold = 0.01f;  //Velocity below this puts object to sleep
		float sleepTimer = 0.0f;       //Time spent inactive before sleeping

		RigidBody(
			GameObjectHandle h,
			const vec3& offsetPosition,
			const vec3& combinedPosition,
			const quat& offsetRotation,
			const quat& combinedRotation,
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
		void ComputeInertiaTensor(const vec3& scale = vec3(1.0f));

		/// <summary>
		/// Assign a new collider or change the existing collider of the rigidbody
		/// </summary>
		void SetCollider(
			const vec3& offsetScale,
			const vec3& combinedScale,
			ColliderType type);

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