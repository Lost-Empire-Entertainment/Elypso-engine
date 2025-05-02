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

#include <vector>

//external
#include "glm.hpp"

namespace KalaKit::Physics::Collision
{
	using glm::vec3;
	using std::vector;

	struct Contact
	{
		vec3 point;
		vec3 normal;
		float penetration;
	};

	struct ContactManifold
	{
		bool colliding = false;
		vector<Contact> contacts;
	};

	struct SATResult
	{
		bool colliding = false;
		vec3 axis = vec3(0.0f);
		float penetration = 0.0f;
	};
}