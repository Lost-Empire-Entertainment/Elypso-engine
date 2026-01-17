//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <initializer_list>
#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "core/kp_registry.hpp"
#include "core/kp_physics_world.hpp"

namespace KalaPhysics::Core
{
	class PhysicsWorld;
}

namespace KalaPhysics::Physics
{
	using std::initializer_list;
	using std::string;
	
	using u8 = uint8_t;
	using u32 = uint32_t;
	using f32 = float;

	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;
	
	using KalaPhysics::Core::KalaPhysicsRegistry;
	using KalaPhysics::Core::MAX_LAYERS;
	
	class LIB_API DelayedRay
	{
		friend class KalaPhysics::Core::PhysicsWorld;
	public:
		static KalaPhysicsRegistry<DelayedRay>& GetRegistry();
		
		//Create a new mask from multiple layers
		static u32 MakeMaskFromLayers(initializer_list<u8> layers);
		
		static DelayedRay* Initialize();

		bool IsInitialized() const;

		u32 GetID() const;
		
		//Set mask directly
		void SetMask(u32 m);
		//Reset mask (no collisions)
		void ClearMask();
		
		//Include layer by name
		void AddLayerToMask(const string& layer);
		//Exclude layer by name
		void RemoveLayerFromMask(const string& layer);
		
		u32 GetMask() const;
		
		~DelayedRay();
	private:
		void Update(f32 deltaTime);

		bool isInitialized{};

		u32 ID{};
	
		u32 mask = ~0ULL; //default - collide with everything
	};
}