//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "core/kw_input.hpp"
#include "graphics/kg_context.hpp"
#include "graphics/kg_shader.hpp"
#include "graphics/kg_camera.hpp"
#include "graphics/kg_texture.hpp"
#include "graphics/kg_material.hpp"
#include "graphics/kg_mesh.hpp"

namespace ElypsoEditor::Graphics
{
    using KalaWindow::Core::Input;
    using KalaGraphics::Graphics::GraphicsContext;
    using KalaGraphics::Graphics::Shader;
    using KalaGraphics::Graphics::Camera;
    using KalaGraphics::Graphics::TextureData;
    using KalaGraphics::Graphics::Texture;
    using KalaGraphics::Graphics::Material;
    using KalaGraphics::Graphics::MeshData;
    using KalaGraphics::Graphics::Mesh;

    using std::string;
    using std::vector;
    using std::filesystem::path;

    class LIB_API Render
    {
    public:
        KNODISCARD
        static bool IsInitialized();
        static void Initialize();

        static void Update();

        KNODISCARD
        static Texture* GetRootTexture();
        static void SetRootTexture(Texture* newValue);
        
        KNODISCARD
        static Texture* GetFallbackTexture();
        static void SetFallbackTexture(Texture* newValue);

        KNODISCARD
        static Texture* GetGroundTexture();
        static void SetGroundTexture(Texture* newValue);

        static void ToggleVSync(
            GraphicsContext* gctx,
            Input* input);

        static void ToggleCamera(Input* input);

        static void MoveCamera(
            Input* input,
            Camera* cam,
            f32 deltaTime);

        static path GetLastImportedMeshPath();
        static vector<Mesh*> GetLastImportedMeshPointers();

        //Returns error string on failure, runs over time,
        //currently triggered by space key
        KNODISCARD
        static string ImportMeshes(
            Input* input,
            Material* material);

        //Returns error string on failure
        //runs once, temporarily combined with ImportMeshes
        KNODISCARD
        static string ExportMeshes(
            const path& targetPath,
            const vector<Mesh*>& meshes);

        KNODISCARD
        static Texture* CreateTexture(
            Shader* shader,
            TextureData&& textureData);

        KNODISCARD
        static Mesh* CreateMesh(
            Material* mat,
            MeshData&& meshData);
    };
}