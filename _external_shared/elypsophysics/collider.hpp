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
#include "gameobjecthandle.hpp"

namespace ElypsoPhysics
{
	using glm::vec3;

	enum class ColliderType
	{
		BOX,
		SPHERE
	};

	class PHYSICS_API Collider
	{
	public:
		ColliderType type;
		GameObjectHandle handle;

		explicit Collider(
			ColliderType type,
			GameObjectHandle h) :
			type(type),
			handle(h) {}
		
		virtual ~Collider() = default;

		Collider(const Collider&) = delete;
		Collider& operator=(const Collider&) = delete;
	};

	class PHYSICS_API BoxCollider : public Collider
	{
	public:
		BoxCollider(GameObjectHandle h, const vec3& size);

		//half size of box in each axis
		vec3 halfExtents;
	};

	class PHYSICS_API SphereCollider : public Collider
	{
	public:
		SphereCollider(GameObjectHandle h, float r);

		float radius;
	};
}