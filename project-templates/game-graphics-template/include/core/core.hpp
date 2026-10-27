//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <array>
#include <filesystem>

#include "core_utils.hpp"

#include "core/kw_input.hpp"
#include "graphics/kg_context.hpp"
#include "graphics/kg_viewport.hpp"
#include "graphics/kg_shader.hpp"
#include "graphics/kg_camera.hpp"

namespace GraphicsTemplate::Core
{
    using KalaWindow::Core::Input;
    using KalaGraphics::Graphics::GraphicsContext;
    using KalaGraphics::Graphics::Viewport;
    using KalaGraphics::Graphics::Shader;
    using KalaGraphics::Graphics::Camera;

    using std::string;
    using std::array;
    using std::filesystem::path;

    class LIB_API GraphicsTemplateCore
    {
    public:
        static bool IsInitialized();
        static void Initialize();

        static const path& GetExePath(); 

        static Input* GetInput();

        static GraphicsContext* GetGraphicsContext();
        static Viewport* GetViewport();
        static Shader* Get3DShader();
        static Shader* Get2DShader();
        static Camera* GetCamera();

        static array<string, 4> GetFPS(f64 secondsToWait);
    };
}