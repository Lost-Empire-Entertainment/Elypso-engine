//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "core/kw_core.hpp"
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

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Vulkan::Vulkan_Global;
using KalaGraphics::Core::GraphicsContext;
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
        GraphicsContext::SetVKInstance(Vulkan_Global::GetInstance());

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
            KalaWindowCore::ForceClose(
                "Elypso Engine window update error",
                "There are no active windows! The program has to shut down.");

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
        for (const auto& w : EngineWindow::GetRegistry().runtimeContent)
        {
            w->Destroy();
        }
    }
}