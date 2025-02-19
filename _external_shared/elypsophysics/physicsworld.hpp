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
#include <memory>

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
	using std::unique_ptr;

	class PHYSICS_API PhysicsWorld
	{
	public:
		static PhysicsWorld& GetInstance();

		void InitializePhysics(const vec3& gravity = vec3(0.0f, -9.81f, 0.0f));
		void ShutdownPhysics();

		/// <summary>
		/// Create a RigidBody and return its handle.
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
		/// Get a RigidBody by handle.
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

		void ResolveCollision(
			RigidBody& bodyA, 
			RigidBody& bodyB, 
			const vec3& collisionNormal,
			const vec3& contactPoint);

		void ApplyFriction(RigidBody& bodyA, RigidBody& bodyB, const vec3& collisionNormal);

		const vec3& GetGravity() const { return gravity; }

	private:
		PhysicsWorld();
		~PhysicsWorld();
		PhysicsWorld(const PhysicsWorld&) = delete;
		PhysicsWorld& operator=(const PhysicsWorld&) = delete;

		bool isInitialized = false;

		vector<unique_ptr<RigidBody>> bodies;                        //Array of all rigidbodies
		unordered_map<GameObjectHandle, size_t, hash<GameObjectHandle>> bodyMap; //Map for quick lookup
		vector<uint32_t> generations;                    //Tracks generation of each index

		vec3 gravity = vec3(0.0f, -9.81f, 0.0f);
	};
}