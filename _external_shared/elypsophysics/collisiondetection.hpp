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

#include <vector>

//external
#include "glm.hpp"

//physics
#include "rigidbody.hpp"
#include "collider.hpp"

namespace ElypsoPhysics
{
	using glm::vec3;
	using std::vector;

	//Single contact point
	struct Contact
	{
		vec3 point;        //World-space contact point
		vec3 normal;       //Normal pointing from bodyA -> bodyB
		float penetration; //Overlap along normal
	};

	//A manifold that can hold multiple contacts
	struct ContactManifold
	{
		bool colliding = false;
		vector<Contact> contacts;
	};

	class PHYSICS_API CollisionDetection
	{
	public:
		static ContactManifold GenerateOBBContactManifold(const RigidBody& a, const RigidBody& b);

		static bool CheckOBBCollision(const RigidBody& a, const RigidBody& b);

		static bool CheckOBBCollisionAt(
			const RigidBody& movingBody, 
			const vec3& futurePosition, 
			const RigidBody& otherBody,
			float deltaTime);
	};
}