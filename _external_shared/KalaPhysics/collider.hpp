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

#include <string>
#include <iostream>

//external
#include "glm.hpp"
#include "gtc/quaternion.hpp"

//physics
#include "gameobjecthandle.hpp"

namespace KalaKit
{
	using glm::vec3;
	using glm::mat3;
	using glm::mat3_cast;
	using glm::quat;
	using std::string;
	using std::to_string;
	using std::cout;

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

		explicit Collider(
			const vec3& offsetScale,
			const vec3& combinedScale,
			ColliderType type, 
			const GameObjectHandle& h);
		
		virtual ~Collider() = default;

		virtual void CalculateBoundingRadius() = 0;
		virtual void UpdateScale(const vec3& newCombinedScale) = 0;

		Collider(const Collider&) = delete;
		Collider& operator=(const Collider&) = delete;
	};

	class KALAPHYSICS_API BoxCollider : public Collider
	{
	public:
		BoxCollider(
			const vec3& offsetScale,
			const vec3& combinedScale,
			const GameObjectHandle& h);
		void UpdateScale(const vec3& newCombinedScale) override
		{
			halfExtents = newCombinedScale * 0.5f;
			CalculateBoundingRadius();
		}
		void CalculateBoundingRadius() override
		{
			boundingRadius = length(halfExtents) * 2.0f * 0.5f; //half diagonal
		}

		//half size of box in each axis
		vec3 halfExtents;
	};

	class KALAPHYSICS_API SphereCollider : public Collider
	{
	public:
		SphereCollider(
			const vec3& offsetScale,
			const vec3& combinedScale,
			const GameObjectHandle& h);
		void UpdateScale(const vec3& newCombinedScale) override
		{
			radius = newCombinedScale.x * 0.5f;
			CalculateBoundingRadius();
		}
		void CalculateBoundingRadius() override
		{
			boundingRadius = radius;
		}

		float radius;
	};
}