//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>

#include "KalaHeaders/core_utils.hpp"

#include "core/kp_registry.hpp"

namespace KalaPhysics::Physics
{
	using std::array;
	
	using u8 = uint8_t;
	using u32 = uint32_t;
	
	using KalaPhysics::Core::KalaPhysicsRegistry;
	
	constexpr u8 MAX_COLLIDERS = 50;
	
	class LIB_API RigidBody
	{
		friend class PhysicsWorld;
	public:
		static inline KalaPhysicsRegistry<RigidBody> registry{};
		
		static RigidBody* Initialize();
		
		//Add a new collider by its ID to this rigidbody
		inline void AddCollider(u32 colliderID)
		{
			//skip if max collider count was reached
			if (colliderCount >= MAX_COLLIDERS) return;
			
			//cannot add what already exists
			for (u8 i = 0; i < colliderCount; i++)
			{
				if (colliders[i] == colliderID) return;
			}
			
			colliders[colliderCount++] = colliderID;
		}
		//Remove an existing collider by ID from this rigidbody
		inline void RemoveCollider(u32 colliderID)
		{
			for (u8 i = 0; i < colliderCount; i++)
			{
				if (colliders[i] == colliderID)
				{
					colliders[i] = colliders[--colliderCount];
					return;
				}
			}
		}
		//Reset colliders count, doesn't waste time removing actual values
		inline void RemoveAllColliders() { colliderCount = 0; }
		
		~RigidBody();
	private:
		array<u32, MAX_COLLIDERS> colliders{};
		u8 colliderCount{};
	};
}