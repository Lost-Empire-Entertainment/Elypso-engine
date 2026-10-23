//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <vector>
#include <bitset>
#include <functional>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kp_registry.hpp"

namespace KalaPhysics::Collision
{
    using KalaHeaders::KalaMath::Transform3D;
    using KalaHeaders::KalaMath::vec3;

    using KalaPhysics::Core::KalaPhysicsRegistry;

    using std::vector;
    using std::bitset;
    using std::function;
	using std::default_delete;

    enum MovementRotationLock : u8
    {
        L_MOVE_X      = 1 << 0,
        L_MOVE_Y      = 1 << 1,
        L_MOVE_Z      = 1 << 2,
        L_MOVE_GLOBAL = 1 << 3, //0 = local, 1 = global
        L_ROT_X       = 1 << 4,
        L_ROT_Y       = 1 << 5,
        L_ROT_Z       = 1 << 6,
        L_ROT_GLOBAL  = 1 << 7  //0 = local, 1 = global
    };

    enum class ColliderType : u8
    {
        CT_SPHERE  = 0,
        CT_AABB    = 1,
        CT_OBB     = 2,
        CT_CAPSULE = 3,
        CT_MESH    = 4
    };

    struct LIB_API Vertex
    {
        //X, Y, Z
        vec3 pos{};
        //X, Y, Z
        vec3 normal{};

        bool operator==(const Vertex& other) const = default;
    };

    class LIB_API Collider
    {
    friend class CollisionManager;
	friend struct default_delete<Collider>;
    public:
        static KalaPhysicsRegistry<Collider>& GetRegistry();

        //Create a new collider, shape defaults to given collider type unless mesh is provided,
        //in that case it defaults to a sphere unless overridden its vertices and indices are overridden
        static Collider* Initialize(
            ColliderType colliderType,
            const Transform3D& transform = {});

        u32 GetID() const;

        const bitset<256>& GetCollisionLayers() const;
        //Override all collision layer states
        void SetCollisionLayers(bitset<256>&& newValue);
        void SetCollisionLayerState(
            u8 newValue,
            bool state);

        bool IsEnabled() const;
        //Can this shape accept collision, disabled by default
        void SetEnabledState(bool newValue);

        bool IsTrigger() const;
        //Does this shape count as a trigger, disabled by default
        void SetTriggerState(bool newValue);

        Transform3D& GetTransform();

        ColliderType GetColliderType() const;
        //Setting the collider type at runtime from any collider except mesh to mesh type
        //will keep the previous collider mesh, but if you set mesh to any other type
        //then the vertices and indices of the mesh shape will be set to that mesh shape
        void SetColliderType(ColliderType newValue);

        const vector<Vertex>& GetVertices() const;
        //Update existing vertices for this collider,
        //only allowed to be called if the collider is a mesh type
        void SetVertices(const vector<Vertex>& newValue);

        const vector<u32>& GetIndices() const;
        //Update existing indices for this collider,
        //only allowed to be called if the collider is a mesh type
        void SetIndices(const vector<u32>& newValue);

        void SetOnCollisionEnterEvent(const function<void(u32)>& newValue);
        void SetOnCollisionStayEvent(const function<void(u32)>& newValue);
        void SetOnCollisionExitEvent(const function<void(u32)>& newValue);
        void SetOnTriggerEnterEvent(const function<void(u32)>& newValue);
        void SetOnTriggerStayEvent(const function<void(u32)>& newValue);
        void SetOnTriggerExitEvent(const function<void(u32)>& newValue);

        void Destroy();
    private:
        ~Collider();

        void Update();

        u32 ID{};
        u8 moveRotLock{};

        bitset<256> collisionLayers{};

        bool isEnabled{};
        bool isTrigger{};

        Transform3D transform{};

        ColliderType colliderType{};
        vector<Vertex> vertices{};
        vector<u32> indices{};

        function<void(u32)> onCollisionEnter{};
        function<void(u32)> onCollisionStay{};
        function<void(u32)> onCollisionExit{};
        function<void(u32)> onTriggerEnter{};
        function<void(u32)> onTriggerStay{};
        function<void(u32)> onTriggerExit{};
    };
}