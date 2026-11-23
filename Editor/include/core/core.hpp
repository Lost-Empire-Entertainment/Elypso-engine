//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <array>
#include <filesystem>

#include "core/kw_input.hpp"
#include "graphics/kg_context.hpp"
#include "graphics/kg_viewport.hpp"
#include "graphics/kg_shader.hpp"
#include "graphics/kg_material.hpp"
#include "graphics/kg_camera.hpp"

namespace ElypsoEditor::Core
{
    using KalaWindow::Core::Input;
    using KalaGraphics::Graphics::GraphicsContext;
    using KalaGraphics::Graphics::Viewport;
    using KalaGraphics::Graphics::Shader;
    using KalaGraphics::Graphics::Material;
    using KalaGraphics::Graphics::Camera;

    using std::string;
    using std::array;
    using std::filesystem::path;

    class LIB_API ElypsoEditorCore
    {
    public:
        KNODISCARD
        static bool IsInitialized();
        static void Initialize();

        KNODISCARD
        static Input* GetInput();

        KNODISCARD
        static GraphicsContext* GetGraphicsContext();
        KNODISCARD
        static Viewport* GetViewport();
        KNODISCARD
        static Shader* Get3DShader();
        KNODISCARD
        static Shader* Get2DShader();
        KNODISCARD
        static Material* GetUnlitMaterial();
        KNODISCARD
        static Material* GetRectMaterial();
        KNODISCARD
        static Material* GetFontMaterial();
        KNODISCARD
        static Camera* GetCamera();

        KNODISCARD
        static array<string, 4> GetFPS();
    };
}