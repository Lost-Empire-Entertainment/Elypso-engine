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
#include "graphics/kg_viewport.hpp"
#include "graphics/kg_shader.hpp"
#include "graphics/kg_camera.hpp"
#include "graphics/kg_texture.hpp"
#include "graphics/kg_material.hpp"
#include "graphics/kg_mesh.hpp"
#include "widgets/kg_text.hpp"
#include "import/kg_import_font.hpp"

namespace ElypsoEditor::Graphics
{
    using KalaWindow::Core::Input;
    using KalaGraphics::Graphics::GraphicsContext;
    using KalaGraphics::Graphics::Viewport;
    using KalaGraphics::Graphics::Shader;
    using KalaGraphics::Graphics::Camera;
    using KalaGraphics::Graphics::TextureData;
    using KalaGraphics::Graphics::Texture;
    using KalaGraphics::Graphics::Material;
    using KalaGraphics::Graphics::MeshData;
    using KalaGraphics::Graphics::Mesh;
    using KalaGraphics::Widgets::Text;
    using KalaGraphics::Import::ImportFont;

    using std::string;
    using std::vector;
    using std::filesystem::path;

    struct TextData
    {
        Text* text{};
        Mesh* textMesh{};
        Mesh* textBackgroundMesh{};
    };

    class LIB_API Render
    {
    public:
        KNODISCARD
        static bool IsInitialized();
        //Main render initialization
        static void Initialize();

        //Main render update loop
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

        //Toggle between three VSync states, press 0, 1 or 2, runs in update
        static void ToggleVSync(
            GraphicsContext* gctx,
            Input* input);

        //Enable/disable camera movement, press ESC to toggle, runs in update
        static void ToggleCamera(Input* input);

        //Camera movement, runs in update
        static void MoveCamera(
            Input* input,
            Camera* cam,
            f32 deltaTime);

        static path GetLastImportedMeshPath();
        static vector<Mesh*> GetLastImportedMeshPointers();

        //Returns error string on failure, runs over time, runs once
        KNODISCARD
        static string ImportMeshes(Shader* shader);

        //Returns error string on failure
        //runs once, temporarily combined with ImportMeshes
        KNODISCARD
        static string ExportMeshes(
            const path& targetPath,
            const vector<Mesh*>& meshes);

        //Returns the text data for the text widget, returns empty struct if not found
        static TextData GetTextData(Text* text);
        //Create a new text, runs once,
        //assign target mesh anchor to anchor this text mesh to that mesh
        static Text* CreateText(
            ImportFont* font,
            Viewport* viewport,
            Mesh* targetMeshAnchor = {});

        //Create a new texture, runs once
        KNODISCARD
        static Texture* CreateTexture(
            Shader* shader,
            TextureData&& textureData);

        //Create a new mesh, runs once
        KNODISCARD
        static Mesh* CreateMesh(
            Material* mat,
            MeshData&& meshData);
    };
}