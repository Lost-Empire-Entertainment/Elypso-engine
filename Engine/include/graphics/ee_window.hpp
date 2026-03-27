//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>
#include <vector>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/ee_registry.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_input.hpp"
#include "opengl/kw_opengl.hpp"
#include "vulkan/kw_vulkan.hpp"
#include "core/kg_context.hpp"

namespace ElypsoEngine::Graphics
{
    using KalaHeaders::KalaMath::vec2;

    using ElypsoEngine::Core::ElypsoRegistry;
    using KalaWindow::Core::Input;
    using KalaWindow::Graphics::ProcessWindow;
    using KalaWindow::Graphics::WindowMode;
    using KalaWindow::Graphics::WindowState;
    using KalaWindow::OpenGL::OpenGL_Context;
    using KalaWindow::Vulkan::Vulkan_Context;
    using KalaGraphics::Core::GraphicsFeature;
    using KalaGraphics::Core::WindowContext;

    using std::string_view;
    using std::vector;

    class LIB_API EngineWindow
    {
    public:
        static ElypsoRegistry<EngineWindow>& GetRegistry();

        //Assign a global Vulkan instance
        static void SetVKInstance(VkInstance instance);
        static VkInstance GetVKInstance();

        //Create a new window
        static EngineWindow* Initialize(
            string_view windowTitle = "UNASSIGNED TITLE",
            vec2 pos = 800,
            vec2 size = 800,
            WindowState state = WindowState::WINDOW_NORMAL,
            WindowMode mode = WindowMode::WINDOWMODE_WINDOWED,
            const vector<GraphicsFeature>& gfxFeatures = { GraphicsFeature::GF_FORCE_OPENGL });

        u32 GetID() const;

        //Get the OS-level window
        ProcessWindow* GetProcessWindow();
        //Get the input context for this window
        Input* GetInput();
        //Get the graphics window context to manage its data
        WindowContext* GetKGContext();
        //Get the OpenGL context of this window
        OpenGL_Context* GetGLContext();
        //Get the Vulkan context of this window
        Vulkan_Context* GetVKContext();
        
        //Single window update
        void Update();

        //Shut down all window content
        void Shutdown();
    private:
        u32 ID{};

        ProcessWindow* window{};
		Input* input{};

        WindowContext* kgCont{};
		OpenGL_Context* glCont{};
        Vulkan_Context* vkCont{};
    };
}