//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>
#include <string_view>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/ee_registry.hpp"
#include "graphics/models/kg_model_primitive.hpp"

namespace ElypsoEngine::Entity
{
    using KalaHeaders::KalaMath::vec3;

    using ElypsoEngine::Core::ElypsoRegistry;

    using KalaGraphics::Graphics::CubeDetails;
    using KalaGraphics::Graphics::PyramidDetails;
    using KalaGraphics::Graphics::SphereDetails;
    using KalaGraphics::Graphics::TorusDetails;

    using std::filesystem::path;
    using std::string;
    using std::string_view;

    using u32 = uint32_t;

    class LIB_API Object
    {
    public:
        static ElypsoRegistry<Object>& GetRegistry();

        static Object* Initialize(
            string_view modelName,
            const path& shaderFolder,
            u32 windowID,
            const vec3& pos,
            const vec3& rot,
            const vec3& size,
            CubeDetails cDet);

        static Object* Initialize(
            string_view modelName,
            const path& shaderFolder,
            u32 windowID,
            const vec3& pos,
            const vec3& rot,
            const vec3& size,
            PyramidDetails pDet);

        static Object* Initialize(
            string_view modelName,
            const path& shaderFolder,
            u32 windowID,
            const vec3& pos,
            const vec3& rot,
            const vec3& size,
            SphereDetails sDet);

        static Object* Initialize(
            string_view modelName,
            const path& shaderFolder,
            u32 windowID,
            const vec3& pos,
            const vec3& rot,
            const vec3& size,
            TorusDetails tDet);

        static Object* Initialize(
            string_view modelName,
            const path& shaderFolder,
            u32 windowID,
            const vec3& pos,
            const vec3& rot,
            const vec3& size,
            const path& modelPath);

        bool SetName(string_view newName);
        const string& GetName() const;

        u32 GetID() const;
        u32 GetContextID() const;
        u32 GetModelID() const;

        void Update();

        ~Object();
    private:
        string name{};

        u32 ID{};
        u32 contextID{};
        u32 modelID{};
    };
}