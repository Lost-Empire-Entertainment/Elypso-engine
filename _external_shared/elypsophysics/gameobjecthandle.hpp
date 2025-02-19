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

#include <cstdint>
#include <functional>

namespace ElypsoPhysics
{
	class PHYSICS_API GameObjectHandle
	{
	public:
		uint32_t index;
		uint32_t generation;

		GameObjectHandle() : index(0), generation(0) {}
		GameObjectHandle(uint32_t idx, uint32_t gen) : index(idx), generation(gen) {}

		bool operator==(const GameObjectHandle& other) const
		{
			return index == other.index
				&& generation == other.generation;
		}
	};
}

namespace std
{
	template <>
	struct hash<ElypsoPhysics::GameObjectHandle>
	{
		size_t operator()(const ElypsoPhysics::GameObjectHandle& handle) const
		{
			size_t h1 = hash<uint32_t>()(handle.index);
			size_t h2 = hash<uint32_t>()(handle.generation);
			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
}