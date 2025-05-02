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

namespace KalaKit::Physics::Core 
{
	class RigidBody;
}

namespace KalaKit::Physics::Collision::Manifold 
{
	struct SATResult;
	struct ContactManifold;
}

#include <string>
#include <iostream>

//external
#include "glm.hpp"
#include "gtc/quaternion.hpp"

//physics
#include "core/gameobjecthandle.hpp"
#include "collision/manifold.hpp"

namespace KalaKit::Physics::Shape
{
	using glm::vec3;
	using glm::mat3;
	using glm::mat3_cast;
	using glm::quat;
	using std::string;
	using std::to_string;
	using std::cout;

	using KalaKit::Physics::Core::GameObjectHandle;
	using KalaKit::Physics::Core::RigidBody;
	using KalaKit::Physics::Collision::ContactManifold;

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
		vec3 offsetScale;             //The offset scale relative to the target gameobject
		vec3 combinedScale;           //The combined scale of target gameobject scale and local offset
		float boundingRadius = 0.0f;

		Collider(
			const vec3& offsetScale,
			const vec3& combinedScale,
			ColliderType type, 
			const GameObjectHandle& h)
			: offsetScale(offsetScale),
			combinedScale(combinedScale),
			type(type),
			handle(h) {}
		
		virtual ~Collider() = default;

		virtual ColliderType GetColliderType() const { return type; }

		virtual ContactManifold GenerateContacts(
			const RigidBody& self,
			const RigidBody& other,
			const Collider& otherCol) const = 0;

		virtual void CalculateBoundingRadius() = 0;
		virtual void UpdateScale(const vec3& newCombinedScale) = 0;

		Collider(const Collider&) = delete;
		Collider& operator=(const Collider&) = delete;
	};
}