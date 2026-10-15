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

namespace KalaPhysics::Collision
{
    using KalaHeaders::KalaMath::Transform3D;
    using KalaHeaders::KalaMath::vec3;

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

    struct LIB_API StationaryData
    {
        //determines resistance to acceleration and collision impulses,
        //clamped from 0.001f to 10000.0f in kilograms
        f32 mass = 1.0f;
        //controls resistance to sliding during contact
        //clamped from 0.0f to 1.0f
        f32 friction = 0.5f;
        //how much will this object bounce,
        //clamped from 0.0f to 1.0f
        f32 restitution = 0.0f;
    };

    struct LIB_API LinearMotionData
    {
        //describes translational motion,
        //clamped from -10000.0f to 10000.0f in meters per second
        vec3 linearVelocity{};
        //accumulates applied forces for the next physics step,
        //clamped from -10000.0f to 10000.0f in newtons
        vec3 force{};
    };

    struct LIB_API AngularMotionData
    {
        //describes rotational motion
        //clamped from -10000.0f to 10000.0f in radians per second
        vec3 angularVelocity{};
        //accumulates rotational forces
        //clamped from -10000.0f to 10000.0f in newton-meters
        vec3 torque{};
    };

    struct LIB_API RuntimeData
    {
        //current linear speed in meters per second
        f32 speed{};
        //current angular speed in radians per second
        f32 angularSpeed{};
        //current linear acceleration in meters per second squared
        f32 linearAcceleration{};
        //current angular acceleration in radians per second squared
        f32 angularAcceleration{};
    };

    class LIB_API Collider
    {
    friend class CollisionHandler;
	friend struct default_delete<Collider>;
    public:
        //Create a new collider,
        //vertices and indices are only allowed to be set
        //if the collider is a mesh type
        static Collider* Initialize(
            ColliderType colliderType,
            const Transform3D& transform,
            const vector<vec3>& vertices = {},
            const vector<u32>& indices = {});

        u32 GetID() const;

        u8 GetMovementRotationLock() const;
        //Set the bitflag values for the movement and rotation lock states.
        //Bit states:
        //- 0: movement x lock
        //- 1: movement y lock
        //- 2: movement z lock
        //- 3: movement state - 0 = local, 1 = global
        //- 4: rotation x lock
        //- 5: rotation y lock
        //- 6: rotation z lock
        //- 7: rotation state - 0 = local, 1 = global
        void SetMovementRotationLock(u8 newValue);

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

        ColliderType GetColliderType() const;
        //Setting the collider type at runtime from any collider except mesh to mesh type
        //will keep the previous collider mesh, but if you set mesh to any other type
        //then the vertices and indices of the mesh shape will be set to that mesh shape
        void SetColliderType(ColliderType newValue);

        Transform3D& GetTransform();

        const vector<vec3>& GetVertices() const;
        //Update existing vertices for this collider,
        //only allowed to be called if the collider is a mesh type
        void SetVertices(const vector<vec3>& newValue);

        const vector<u32>& GetIndices() const;
        //Update existing indices for this collider,
        //only allowed to be called if the collider is a mesh type
        void SetIndices(const vector<u32>& newValue);

        //Returns the value for who collided against this collider
        void OnCollisionEvent(const function<void(u32)>& newValue);
        //Returns the value for who started triggering with this collider
        void OnTriggerEnterEvent(const function<void(u32)>& newValue);
        //Returns the value for who stayed within the trigger of this collider
        void OnTriggerStayEvent(const function<void(u32)>& newValue);
        //Returns the value for who stopped triggered with this collider
        void OnTriggerExitEvent(const function<void(u32)>& newValue);

        StationaryData& GetStationaryData();
        LinearMotionData& GetLinearMotionData();
        AngularMotionData& GetAngularMotionData();
        const RuntimeData& GetRuntimeData() const;

        void Destroy();
    private:
        ~Collider();

        void Update();

        u32 ID{};
        u8 moveRotLock{};

        bitset<256> collisionLayers{};

        bool isEnabled{};
        bool isTrigger{};

        ColliderType colliderType{};
        Transform3D transform{};
        Transform3D lastTransform{};
        vector<vec3> vertices{};
        vector<u32> indices{};

        function<void(u32)> collisionEvent{};
        function<void(u32)> triggerEnterEvent{};
        function<void(u32)> triggerStayEvent{};
        function<void(u32)> triggerExitEvent{};

        StationaryData stationaryData{};
        LinearMotionData linearMotionData{};
        AngularMotionData angularMotionData{};
        RuntimeData runtimeData{};
    };
}