//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "graphics/kw_window_global.hpp"
#include "vulkan/kw_vulkan.hpp"
#ifdef __linux__
#include "core/kw_messageloop_x11.hpp"
#endif

#include "graphics/ee_render.hpp"
#include "graphics/ee_window.hpp"
#include "core/kg_context.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Graphics::Window_Global;
using KalaWindow::Vulkan::Vulkan_Global;
using KalaGraphics::Core::WindowContext;
#ifdef __linux__
using KalaWindow::Core::MessageLoop;
#endif

namespace ElypsoEngine::Graphics
{
    static bool isInitialized{};

    void Render::Initialize()
    {
        if (isInitialized)
        {
            Log::Print(
                "Cannot initialize Elypso Engine render structure more than once!",
                "EE_RENDER",
                LogType::LOG_ERROR,
                2);

            return;
        }

        Window_Global::Initialize();

        Vulkan_Global::Initialize();
        WindowContext::SetVKInstance(Vulkan_Global::GetInstance());

        Log::Print(
            "Initialized render!",
            "EE_RENDER",
            LogType::LOG_SUCCESS);
    }

    bool Render::IsInitialized() { return isInitialized; }

    void Render::Update()
    {
        if (EngineWindow::GetRegistry().runtimeContent.empty())
        {
            Log::Print(
                "There are no windows to render! Did you forget to initialize a window?",
                "EE_RENDER",
                LogType::LOG_WARNING);

            return;
        }

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
            "EE_RENDER",
            LogType::LOG_INFO);

        for (const auto& w : EngineWindow::GetRegistry().runtimeContent)
        {
            w->Shutdown();
        }
    }
}