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
		/// Shuts down the physics engine and cleans up all allocated resources
		/// </summary>
		void ShutdownPhysics();

		/// <summary>
		/// Create a RigidBody and return its handle
		/// </summary>
		GameObjectHandle CreateRigidBody(
			const vec3& pos, 
			const quat& rot,
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
		/// Remove a RigidBody
		/// </summary>
		void RemoveRigidBody(GameObjectHandle handle);

		/// <summary>
		/// Update physics simulation
		/// </summary>
		void StepSimulation(float deltaTime);

		/// <summary>
		/// Resolves a collision by applying impulse forces to separate the bodies and simulate realistic response
		/// </summary>
		void ResolveCollision(
			RigidBody& bodyA, 
			RigidBody& bodyB, 
			const vec3& collisionNormal,
			const vec3& contactPoint);

		/// <summary>
		/// Applies frictional forces to reduce sliding and simulate surface resistance after a collision
		/// </summary>
		void ApplyFriction(RigidBody& bodyA, RigidBody& bodyB, const vec3& collisionNormal);

		const vec3& GetGravity() const { return gravity; }

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

		//Global gravity
		vec3 gravity;
	};
}