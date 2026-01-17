//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <initializer_list>
#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "core/kp_physics_world.hpp"

namespace KalaPhysics::Core
{
	class PhysicsWorld;
}

namespace KalaPhysics::Physics
{
	using std::initializer_list;
	using std::string;

	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;
	
	using KalaPhysics::Core::MAX_LAYERS;
	
	constexpr f32 MAX_DISTANCE = 10000.0f;

	class Collider; //forward declare collider so Ray can return Collider
	
	class LIB_API Ray
	{
		friend class KalaPhysics::Core::PhysicsWorld;
	public:
		//Create a new mask from multiple layers
		static u32 MakeMaskFromLayers(initializer_list<u8> layers);
		
		//Returns true if this ray hit any collider with the valid layer,
		//a maxDistance of 0.0f means ray max distance is 10000 units
		static bool HitAny(
			const vec3& origin,
			const vec3& direction,
			f32 maxDistance = 0.0f);
			
		//Returns the collider non-owning pointer this ray hit,
		//a maxDistance of 0.0f means ray max distance is 10000 units
		static Collider* HitCollider(
			const vec3& origin,
			const vec3& direction,
			f32 maxDistance = 0.0f);
		
		//Set mask directly
		void SetMask(u32 m);
		//Reset mask (no collisions)
		void ClearMask();
		
		//Include layer by name
		void AddLayerToMask(const string& layer);
		//Exclude layer by name
		void RemoveLayerFromMask(const string& layer);
		
		u32 GetMask() const;
	private:
		u32 mask = ~0ULL; //default - collide with everything
	};
}