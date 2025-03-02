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

#include <vector>
#include <unordered_map>
#include <cstdint>

//external
#include "glm.hpp"

//physics
#include "rigidbody.hpp"

namespace ElypsoPhysics
{
	using std::vector;
	using std::unordered_map;
	using glm::vec3;
	using glm::quat;
	using std::hash;

	class PHYSICS_API PhysicsWorld
	{
	public:
		static PhysicsWorld& GetInstance();

		/// <summary>
		/// Initializes the physics engine with the specified gravity vector
		/// </summary>
		void InitializePhysics(const vec3& gravity = vec3(0.0f, -9.81f, 0.0f));

		/// <summary>
		/// Create a RigidBody and return its handle
		/// </summary>
		GameObjectHandle CreateRigidBody(
			const vec3& offsetPosition,
			const vec3& combinedPosition,
			const quat& offsetRotation,
			const quat& combinedRotation,
			ColliderType colliderType,
			const vec3& colliderSizeOrRadius,
			float mass = 1.0f,
			float restitution = 0.5f,
			float staticFriction = 0.5f,
			float dynamicFriction = 0.3f,
			float gravityFactor = 1.0f,
			bool useGravity = true);

		/// <summary>
		/// Get a RigidBody by handle
		/// </summary>
		RigidBody* GetRigidBody(const GameObjectHandle& handle);

		/// <summary>
		/// Remove a RigidBody by handle
		/// </summary>
		void RemoveRigidBody(const GameObjectHandle& handle);

		/// <summary>
		/// Update physics simulation
		/// </summary>
		void StepSimulation(float deltaTime);

		void ApplyPhysicsIntegration(float deltaTime);

		/// <summary>
		/// Resolves a collision by applying impulse forces to separate the bodies and simulate realistic response
		/// </summary>
		void ResolveCollision(
			RigidBody& bodyA, 
			RigidBody& bodyB, 
			const vec3& collisionNormal,
			const vec3& contactPoint,
			float penetration);

		/// <summary>
		/// Applies frictional forces to reduce sliding and simulate surface resistance after a collision
		/// </summary>
		void ApplyFriction(
			RigidBody& bodyA, 
			RigidBody& bodyB, 
			const vec3& collisionNormal,
			const vec3& contactPoint) const;

		void SetGravity(const vec3& newGravity) { gravity = newGravity; }
		void SetAngularDamping(float value) { angularDamping = value; }
		void SetLowAngularVelocityFactor(float value) { lowAngularVelocityFactor = value; }
		void SetFrictionMultiplier(float value) { frictionMultiplier = value; }
		void SetCorrectionFactor(float value) { correctionFactor = value; }
		void SetMinPenetrationThreshold(float value) { minPenetrationThreshold = value; }

		const vec3& GetGravity() const { return gravity; }
		float GetAngularDamping() const { return angularDamping; }
		float GetLowAngularVelocityFactor(float value) const { return lowAngularVelocityFactor; }
		float GetFrictionMultiplier() const { return frictionMultiplier; }
		float GetCorrectionFactor() const { return correctionFactor; }
		float GetMinPenetrationThreshold() const { return minPenetrationThreshold; }

		const vector<RigidBody*>& GetBodies() const { return bodies; }
		const unordered_map<GameObjectHandle, size_t, hash<GameObjectHandle>>& GetBodyMap() const { return bodyMap; }
		const vector<uint32_t>& GetGenerations() const { return generations; }

	private:
		PhysicsWorld();
		~PhysicsWorld();
		PhysicsWorld(const PhysicsWorld&) = delete;
		PhysicsWorld& operator=(const PhysicsWorld&) = delete;

		bool isInitialized = false;

		//Array of all active RigidBody instances managed by the physics world
		vector<RigidBody*> bodies;
		//Maps GameObjectHandle to its corresponding index in the 'bodies' array
		unordered_map<GameObjectHandle, size_t, hash<GameObjectHandle>> bodyMap;
		//Tracks the generation count for each index in the 'bodies' array
		vector<uint32_t> generations;

		vec3 gravity;                          //Global gravity
		float angularDamping = 0.998f;         //Controls how quickly rotation slows down
		float lowAngularVelocityFactor = 0.5f; //How much to slow rotation when velocity is very low
		float frictionMultiplier = 0.1f;       //Global friction multiplier
		float correctionFactor = 0.2f;         //Strength of positional correction
		float minPenetrationThreshold = 0.01f; //Minimum penetration depth to trigger correction
	};
}