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

//external
#include "glm.hpp"

//physics
#include "rigidbody.hpp"
#include "collider.hpp"

namespace ElypsoPhysics
{
	using glm::vec3;

	class PHYSICS_API CollisionDetection
	{
	public:
		static bool CheckAABBCollision(const RigidBody& a, const RigidBody& b);

		static bool CheckBoxBoxCollision(
			const BoxCollider& boxA,
			const vec3& posA,
			const BoxCollider& boxB,
			const vec3& posB);

		static bool CheckSphereSphereCollision(
			const SphereCollider& sphereA,
			const vec3& posA,
			const SphereCollider& sphereB,
			const vec3& posB);
	};
}