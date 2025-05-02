//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAPHYSICS_DLL_EXPORT
		#define KALAPHYSICS_API __declspec(dllexport)
	#else
		#define KALAPHYSICS_API __declspec(dllimport)
	#endif
#else
	#define KALAPHYSICS_API
#endif

#include <cstdint>
#include <functional>

namespace KalaKit::Physics::Core
{
	class KALAPHYSICS_API GameObjectHandle
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
	using KalaKit::Physics::Core::GameObjectHandle;

	template <>
	struct hash<GameObjectHandle>
	{
		size_t operator()(const GameObjectHandle& handle) const
		{
			size_t h1 = hash<uint32_t>()(handle.index);
			size_t h2 = hash<uint32_t>()(handle.generation);
			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
}