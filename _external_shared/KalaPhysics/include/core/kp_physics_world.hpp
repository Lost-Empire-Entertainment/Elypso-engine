//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>
#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

namespace KalaPhysics::Core
{
	using std::array;
	using std::string;
	using std::to_string;
	
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;

	//total max allowed substeps
	constexpr u8 MAX_SUBSTEPS = 12;
	//how fast substeps grow exponentially
	constexpr f32 SUBSTEP_GROWTH_FACTOR = 1.25f;
	//at how many registered collisions do we start increasing substeps
	constexpr u8 COLLISION_THRESHOLD = 10;
	
	//32 layers fit in a 32-bit bitmask for uint32_t for bitmasking collisions and colliders
	constexpr u8 MAX_LAYERS = 32;
	constexpr u8 MAX_LAYER_NAME_LENGTH = 50;

	inline const vec3 MAX_GRAVITY = 100.0f;
	
	class LIB_API PhysicsWorld
	{
	public:
		//The main physics update function that handles a
		//single simulation step per call based off of the passed deltaTime variable.
		//Substeps are adjusted internally dynamically based off of registered collisions per update call,
		//modify MAX_SUBSTEPS, SUBSTEP_GROWTH_FACTOR and COLLISION_THRESHOLD to adjust substep growth
		static void Update(f32 deltaTime);

		//Returns count of currently used layers
		static u64 GetLayerCount();

		//Add a new layer
		static void AddLayer(const string& layer);
		//Remove an existing layer
		static void RemoveLayer(const string& layer);
		//Reset layers
		static void RemoveAllLayers();
		
		//Get layer name (or "NONE" if not found)
		static const string& GetLayer(u8 layer);
		//Get layer index (or 255 if not found)
		static u8 GetLayer(const string& layer);
		
		//Enable/disable collision between layers
		static void SetCollisionRule(
			u8 a,
			u8 b,
			bool value);
		//Check if both layers can collide
		static bool CanCollide(
			u8 a,
			u8 b);

		static const vec3& GetGravity();
		static void SetGravity(const vec3& newValue);
	};
}