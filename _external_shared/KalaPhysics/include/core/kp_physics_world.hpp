//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>
#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

namespace KalaPhysics::Core
{
	using std::array;
	using std::string;
	
	using u8 = uint8_t;
	
	using KalaHeaders::KalaMath::vec3;
	
	//64 layers fit in a 64-bit bitmask for uint64_t for bitmasking collisions and colliders
	constexpr u8 MAX_LAYERS = 64;
	constexpr u8 MAX_LAYER_LENGTH = 50;
	
	const vec3 GRAVITY = vec3(0.0f, -9.81f, 0.0f);
	
	class LIB_API PhysicsWorld
	{
	public:
		//Add a new layer
		static inline void AddLayer(const string& layer)
		{
			//skip if max layer count was reached
			if (layerCount >= MAX_LAYERS
				|| layer == "NONE")
			{
				return;
			}
			
			//cannot add what already exists
			for (u8 i = 0; i < layerCount; i++)
			{
				if (layers[i] == layer) return;
			}
			
			string clamped = layer;
			if (clamped.size() > MAX_LAYER_LENGTH) clamped.resize(MAX_LAYER_LENGTH);
			
			layers[layerCount++] = clamped;
		}
		//Remove an existing layer
		static inline void RemoveLayer(const string& layer)
		{
			for (u8 i = 0; i < layerCount; i++)
			{
				if (layers[i] == layer)
				{
					layers[i] = layers[--layerCount];
					return;
				}
			}
		}
		//Reset layers
		static inline void RemoveAllLayers()
		{ 
			for (auto& l : layers) l.clear();
			layerCount = 0;
		}
		
		//Get layer name (or "NONE" if not found)
		static inline const string& GetLayer(u8 layer)
		{
			static string none = "NONE";
			
			if (layer >= layerCount) return none;
			
			return layers[layer];
		}
		//Get layer index (or 255 if not found)
		static inline u8 GetLayer(const string& layer)
		{
			for (u8 i = 0; i < layerCount; i++)
			{
				if (layers[i] == layer) return i;
			}
			
			return 255;
		}
		
		//Enable/disable collision between layers
		static inline void SetCollisionRule(
			u8 a,
			u8 b,
			bool value)
		{
			if (a >= layerCount
				|| b >= layerCount)
			{
				return;
			}
			
			collisionMatrix[a][b] = value;
			collisionMatrix[b][a] = value;
		}
		//Check if both layers can collide
		static inline bool CanCollide(
			u8 a,
			u8 b)
		{
			if (a >= layerCount
				|| b >= layerCount)
			{
				return false;
			}
			
			return collisionMatrix[a][b];
		}
	private:
		static inline array<string, MAX_LAYERS> layers{};
		static inline u8 layerCount{};
		
		static inline bool collisionMatrix[MAX_LAYERS][MAX_LAYERS]{};
	};
}