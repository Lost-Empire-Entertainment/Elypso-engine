//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "graphics/kw_window_global.hpp"
#include "opengl/kw_opengl.hpp"
#include "vulkan/kw_vulkan.hpp"
#include "core/kg_context.hpp"
#ifdef __linux__
#include "core/kw_messageloop_x11.hpp"
#endif

#include "graphics/ee_render.hpp"
#include "graphics/ee_window.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Graphics::Window_Global;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::Vulkan::Vulkan_Global;
using KalaGraphics::Core::WindowContext;
#ifdef __linux__
using KalaWindow::Core::MessageLoop;
#endif

namespace ElypsoEngine::Graphics
{
    static bool isInitialized{};
    static bool allowGL{};
    static bool allowVK{};

    void Render::Initialize(
        bool newAllowGL,
        bool newAllowVK)
    {
        if (isInitialized)
        {
            Log::Print(
                "Cannot initialize Elypso Engine render structure more than once!",
                "ELYPSO_RENDER",
                LogType::LOG_ERROR,
                2);

            return;
        }

        allowGL = newAllowGL;
        allowVK = newAllowVK;

        Window_Global::Initialize();
        if (allowGL) OpenGL_Global::Initialize();
        if (allowVK)
        {
            Vulkan_Global::Initialize();

            EngineWindow::SetVKInstance(Vulkan_Global::GetInstance());
            WindowContext::SetVKInstance(Vulkan_Global::GetInstance());
        }

        Log::Print(
            "Initialized render!",
            "ELYPSO_RENDER",
            LogType::LOG_SUCCESS);
    }

    bool Render::IsInitialized() { return isInitialized; }

    bool Render::AllowGL() { return allowGL; }
    bool Render::AllowVK() { return allowVK; }

    void Render::Update()
    {
        static bool sentWarning{};
        if (EngineWindow::GetRegistry().runtimeContent.empty())
        {
            if (!sentWarning)
            {
                Log::Print(
                    "There are no windows to render! Did you forget to initialize a window?",
                    "ELYPSO_RENDER",
                    LogType::LOG_WARNING);

                sentWarning = true;
            }
        }
        else { if (sentWarning) sentWarning = false; }

#ifdef __linux__
        MessageLoop::Update();
#endif

        for (const auto& w : EngineWindow::GetRegistry().runtimeContent)
        {
            w->Update();
        }
    }

    void Render::Shutdown()
    {
        Log::Print(
            "Shutting down Elypso Engine render structure.",
            "ELYPSO_RENDER",
            LogType::LOG_INFO);

        for (const auto& w : EngineWindow::GetRegistry().runtimeContent)
        {
            w->Shutdown();
        }
    }
}