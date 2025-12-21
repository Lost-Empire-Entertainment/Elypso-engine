//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>
#include <functional>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/kp_registry.hpp"
#include "core/kp_physics_world.hpp"

namespace KalaPhysics::Physics
{
	using std::vector;
	using std::string;
	using std::function;
	
	using KalaPhysics::Core::KalaPhysicsRegistry;
	using KalaPhysics::Core::PhysicsWorld;
	
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::quat;
	using KalaHeaders::KalaMath::Transform3D;
	
	class LIB_API Collider
	{	
		friend class PhysicsWorld;
	public:
		static inline KalaPhysicsRegistry<Collider> registry{};
		
		enum class ColliderShape : u8
		{
			COLLIDER_NONE = 0, //default state of all colliders, no collision will occur
			
			COLLIDER_BSP = 1,  //bounding sphere
			COLLIDER_AABB = 2, //axis-aligned bounding box
			COLLIDER_OBB = 3,  //oriented bounding box
			COLLIDER_BCP = 4,  //bounding capsule
			
			COLLIDER_KDOP_10_X = 5, //10-face Z-axis discrete oriented polytope
			COLLIDER_KDOP_10_Y = 6, //10-face Y-axis discrete oriented polytope
			COLLIDER_KDOP_10_Z = 7, //10-face Z-axis discrete oriented polytope
			COLLIDER_KDOP_18 = 8,   //18-face discrete oriented polytope
			COLLIDER_KDOP_26 = 9,   //26-face discrete oriented polytope
			
			COLLIDER_BCH = 10 //bounding convex hull
		};
		
		enum class ColliderType : u8
		{
			COLLIDER_TYPE_NONE = 0, //default unassigned target type
			
			COLLIDER_TYPE_BP = 1, //broadphase
			COLLIDER_TYPE_NP = 2  //narrowphase
		};
		
		static Collider* Initialize(
			ColliderShape shape,
			ColliderType type,
			u32 parentRigidbody,
			const vector<vec3>& vertices,
			const Transform3D& transform);
			
		inline void SetTriggerState(bool newValue) { isTrigger = newValue; }
		inline bool IsTrigger() const { return isTrigger; }
			
		//Assign a layer to this collider by index
		inline void SetLayer(u8 newLayer)
		{
			if (PhysicsWorld::GetLayer(newLayer) == "NONE") return;
			
			layer = newLayer;
		}
		//Assign a layer to this collider by name
		inline void SetLayer(const string& newLayer)
		{
			u8 foundLayer = PhysicsWorld::GetLayer(newLayer);
			if (foundLayer == 255) return;
			
			layer = foundLayer;
		}
		//Reset existing layer to default - no collisions accepted
		inline void ClearLayer() { layer = 0; }
		
		inline u8 GetLayer() const { return layer; }
		
		inline void SetOnTriggerEnter(const function<void()>& func) { onTriggerEnter = func; }
		inline void SetOnTriggerExit(const function<void()>& func) { onTriggerExit = func; }
		inline void SetOnTriggerStay(const function<void()>& func) { onTriggerStay = func; }
		
		inline void ClearOnTriggerEnter() { onTriggerEnter = nullptr; }
		inline void ClearOnTriggerExit() { onTriggerExit = nullptr; }
		inline void ClearOnTriggerStay() { onTriggerStay = nullptr; }
		
		~Collider();
	private:
		void Update(f32 deltaTime);
	
		bool isTrigger{};
	
		u8 layer{};
		
		function<void()> onTriggerEnter{};
		function<void()> onTriggerExit{};
		function<void()> onTriggerStay{};
	};
}