//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <bitset>
#include <array>
#include <vector>
#include <string>

#include "core_utils.hpp"
#include "math_utils.hpp"

namespace KalaPhysics::Collision
{
    using KalaHeaders::KalaMath::vec3;

    using std::bitset;
    using std::array;
    using std::vector;
    using std::pair;
    using std::string;

    using IgnorePair = pair<u32, u32>;
    using IgnoreList = vector<IgnorePair>;

    static constexpr vec3 DEFAULT_GLOBAL_GRAVITY = { 0.0f, -9.81f, 0.0f };

    struct CollisionMatrix
    {
        array<bitset<256>, 256> layers{};
        array<string, 256> layerNames{};
    };

    class LIB_API CollisionHandler
    {
    public:
        static bool IsInitialized();
        static void Initialize();

        //Call once per frame, set to fixed update for correct results across framerate jumps,
        //ideally at 60 frames per second, updates all colliders internally on its own,
        //higher substep count improves simulation accuracy but costs more,
        //clamped from 1 to 10, defaults to 1
        static void Update(u8 substeps = 1);

        static const vec3& GetGlobalGravity();
        //Overwrite global gravity,
        //defaults to { 0.0f, -9.81f, 0.0f },
        //+y is global up direction
        static void SetGlobalGravity(const vec3& newValue);

        static const CollisionMatrix& GetCollisionMatrix();
        //Overwrite global collision matrix bitset,
        //defaults all collider layers to off
        static void SetCollisionMatrix(CollisionMatrix&& newValue);

        static const IgnoreList& GetIgnoreList();
        //Override existing ignore list
        static void SetIgnoreList(IgnoreList&& newValue);
        static void AddToIgnoreList(IgnorePair&& newValue);
        static void RemoveFromIgnoreList(IgnorePair&& newValue);

        static void Shutdown();
    };
}