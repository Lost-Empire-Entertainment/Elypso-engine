//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kp_registry.hpp"

namespace KalaPhysics::Collision
{
    using KalaHeaders::KalaMath::Transform3D;
    using KalaHeaders::KalaMath::vec3;

    using KalaPhysics::Core::KalaPhysicsRegistry;

    using std::default_delete;

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

    class LIB_API RigidBody
    {
    friend class CollisionManager;
	friend struct default_delete<RigidBody>;
    public:
        static KalaPhysicsRegistry<RigidBody>& GetRegistry();

        static RigidBody* Initialize(const Transform3D& transform);

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

        Transform3D& GetTransform();

        StationaryData& GetStationaryData();
        LinearMotionData& GetLinearMotionData();
        AngularMotionData& GetAngularMotionData();
        const RuntimeData& GetRuntimeData() const;

        void Destroy();
    private:
        ~RigidBody();

        void Update();

        u32 ID{};

        u8 moveRotLock{};
        Transform3D transform{};

        StationaryData stationaryData{};
        LinearMotionData linearMotionData{};
        AngularMotionData angularMotionData{};
        RuntimeData runtimeData{};
    };
}