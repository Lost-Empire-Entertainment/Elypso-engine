//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <initializer_list>
#include <string>

#include "KalaHeaders/core_utils.hpp"

#include "core/kp_registry.hpp"
#include "core/kp_physics_world.hpp"

namespace KalaPhysics::Physics
{
	using std::initializer_list;
	using std::string;
	
	using u8 = uint8_t;
	using u64 = uint64_t;
	using f32 = float;
	
	using KalaPhysics::Core::KalaPhysicsRegistry;
	using KalaPhysics::Core::PhysicsWorld;
	using KalaPhysics::Core::MAX_LAYERS;
	
	class LIB_API DelayedRay
	{
		friend class PhysicsWorld;
	public:
		static inline KalaPhysicsRegistry<DelayedRay> registry{};
		
		//Create a new mask from multiple layers
		static inline u64 MakeMaskFromLayers(initializer_list<u8> layers)
		{
			u64 m = 0ULL;
			for (u8 l : layers)
			{
				if (l < MAX_LAYERS) m |= (1ULL << l);
			}
			
			return m;
		}
		
		static DelayedRay* Initialize();
		
		//Set mask directly
		inline void SetMask(u64 m) { mask = m; }
		//Reset mask (no collisions)
		inline void ClearMask() { mask = 0ULL; }
		
		//Include layer by index
		inline void AddLayerToMask(u8 layer)
		{
			if (PhysicsWorld::GetLayer(layer) == "NONE") return;
			
			mask |= (1ULL << layer);
		}
		//Exclude layer by index
		inline void RemoveLayerFromMask(u8 layer)
		{
			if (PhysicsWorld::GetLayer(layer) == "NONE") return;
			
			mask &= ~(1ULL << layer);
		}
		
		//Include layer by name
		inline void AddLayerToMask(const string& layer)
		{
			u8 foundLayer = PhysicsWorld::GetLayer(layer);
			if (foundLayer == 255) return;
			
			mask |= (1ULL << foundLayer);
		}
		//Exclude layer by name
		inline void RemoveLayerFromMask(const string& layer)
		{
			u8 foundLayer = PhysicsWorld::GetLayer(layer);
			if (foundLayer == 255) return;
			
			mask &= ~(1ULL << foundLayer);
		}
		
		inline u64 GetMask() const { return mask; }
		
		~DelayedRay();
	private:
		void Update(f32 deltaTime);
	
		u64 mask = ~0ULL; //default - collide with everything
	};
}