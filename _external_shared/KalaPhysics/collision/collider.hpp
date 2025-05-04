//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//external
#include "glm.hpp"

//physics
#include "core/utils.hpp"
#include "core/gameobjecthandle.hpp"

namespace KalaKit::Physics::Collision
{
	using glm::vec3;

	using KalaKit::Physics::Core::GameObjectHandle;

	enum class ColliderType
	{
		BOX,
		SPHERE
	};

	class KALAPHYSICS_API Collider
	{
	public:
		ColliderType type;
		GameObjectHandle handle;
		float boundingRadius = 0.0f;

		explicit Collider(
			ColliderType type, 
			const GameObjectHandle& h);
		
		virtual ~Collider() = default;

		Collider(const Collider&) = delete;
		Collider& operator=(const Collider&) = delete;
	};

	class KALAPHYSICS_API BoxCollider : public Collider
	{
	public:
		BoxCollider(const GameObjectHandle& h);

		//half size of box in each axis
		vec3 halfExtents = vec3(0);
	};

	class KALAPHYSICS_API SphereCollider : public Collider
	{
	public:
		SphereCollider(const GameObjectHandle& h);

		float radius = 0.0f;
	};
}