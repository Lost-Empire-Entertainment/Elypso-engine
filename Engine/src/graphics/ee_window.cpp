//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>
#include <vector>

#include "log_utils.hpp"
#include "core_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "core/kg_context.hpp"
#include "opengl/kw_opengl.hpp"
#include "vulkan/kw_vulkan.hpp"
#ifdef __linux__
#include "graphics/kw_window_global.hpp"
#endif

#include "graphics/ee_window.hpp"
#include "graphics/ee_render.hpp"
#include "core/ee_core.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaCore::ContainsValue;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::WindowData;
using KalaWindow::OpenGL::OpenGL_Context;
using KalaWindow::Vulkan::Vulkan_Context;
using KalaGraphics::Core::GraphicsFeature;
using KalaGraphics::Core::WindowContext;
using KalaGraphics::Core::WindowContextData;
using KalaGraphics::Core::FramebufferSize;
#ifdef __linux__
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;
#endif

using ElypsoEngine::Graphics::Render;
using ElypsoEngine::Core::EngineCore;

using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::vector;

namespace ElypsoEngine::Graphics
{
	static ElypsoRegistry<EngineWindow> registry{};

    static void ShutdownCallback(u32 windowID)
    {
        EngineWindow* enwin = registry.GetContent(windowID);
        if (enwin == nullptr)
        {
            Log::Print("Failed to call shutdown callback because the engine window ID was not found!",
                "ELYPSO_WINDOW",
                LogType::LOG_ERROR,
                2);

            return;
        }
        
        ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(enwin->GetWindowID());
        if (!pw)
        {
            Log::Print(
                "Failed to call shutdown callback because the window ID was not found!",
                "ELYPSO_WINDOW",
                LogType::LOG_ERROR,
                2);

            return;
        }

        Log::Print(
            "Closing Elypso Engine window '" + pw->GetTitle() + "'.",
            "ELYPSO_WINDOW",
            LogType::LOG_INFO);

        WindowContext* kgctx = WindowContext::GetRegistry().GetContent(enwin->GetContextID());
        if (!kgctx)
        {
            Log::Print(
                "Failed to call shutdown callback because the context ID was not found!",
                "ELYPSO_WINDOW",
                LogType::LOG_ERROR,
                2);

            return;
        }

        kgctx->Shutdown();

        registry.RemoveContent(enwin->GetID());
    }

	ElypsoRegistry<EngineWindow>& EngineWindow::GetRegistry() { return registry; }

    EngineWindow* EngineWindow::Initialize(
        string_view windowTitle,
        vec2 pos,
        vec2 size,
        WindowState state,
        WindowMode mode,
        const vector<GraphicsFeature>& gfxFeatures)
    {
        if (windowTitle.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine window init error",
                "Window title cannot be empty!");
        }
        if (windowTitle.size() > 50)
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine window init error",
                "Window title is too long!");
        }

        string title = string(windowTitle);

        unique_ptr<EngineWindow> newWindow = make_unique<EngineWindow>();
        EngineWindow* windowPtr = newWindow.get();

        ProcessWindow* pw = ProcessWindow::Initialize(windowTitle);
        pw->SetClientRectSize(size);
        pw->SetPosition(pos);

        u32 windowID = pw->GetID();

        Input::Initialize(windowID);

        OpenGL_Context* glctx{};
        Vulkan_Context* vkctx{};

        if (!ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_SOFTWARE)
            && !ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_VULKAN)
            && Render::AllowGL()
            && (gfxFeatures.empty()
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_OPENGL)))
        {
            glctx = OpenGL_Context::Initialize(windowID);
        }
        if (!ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_SOFTWARE)
            && !ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_OPENGL)
            && !glctx
            && Render::AllowVK()
            && (gfxFeatures.empty()
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_VULKAN)
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_COMPUTE_SHADERS)
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_RAY_TRACING)
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_PATH_TRACING)))
        {
            vkctx = Vulkan_Context::Initialize(windowID);
        }

        const WindowData& wData = pw->GetWindowData();
#ifdef _WIN32
        WindowContextData kgData =
        {
            .windowID = windowID,
            .isFramebufferDynamic = true,
            .fbSize = FramebufferSize::FB_1920_1080,
            .context_window = wData.window
        };
#else
        const X11GlobalData& data = Window_Global::GetGlobalData();

        WindowContextData kgData =
        {
            .windowID = windowID,
            .isFramebufferDynamic = true,
            .fbSize = FramebufferSize::FB_1920_1080,
            .context_display = data.display,
            .context_window = wData.window
        };
#endif

        if (glctx) kgData.context_gl = glctx->GetContext();
        if (vkctx) kgData.context_vk_surface = vkctx->GetSurface();

        //pre-sync to ensure kg gets the highest id
        EngineCore::SyncID();

        WindowContext* kgctx = WindowContext::Initialize(kgData, gfxFeatures);

        pw->SetRedrawCallback([kgctx](){ kgctx->Update(); });
        pw->SetResizeCallback([kgctx](){ kgctx->ResizeUpdate(); });

        pw->SetWindowState(state);
        pw->SetWindowMode(mode);

        //post-sync to ensure ee gets the highest id from kw
        EngineCore::SyncID();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        windowPtr->ID = newID;
        windowPtr->windowID = windowID;
        windowPtr->contextID = kgctx->GetID();

        pw->SetShutdownCallback([newID](){ ShutdownCallback(newID); });

        registry.AddContent(newID, std::move(newWindow));

        Log::Print(
			"Created new window '" + string(windowTitle) + "' with ID '" + to_string(newID) + "'!",
			"ELYPSO_WINDOW",
			LogType::LOG_SUCCESS);

        return windowPtr;
    }

    u32 EngineWindow::GetID() const { return ID; }
    u32 EngineWindow::GetWindowID() const { return windowID; }
    u32 EngineWindow::GetContextID() const { return contextID; }

    void EngineWindow::Update()
    {
        ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(windowID);
        if (!pw)
        {
            KalaWindowCore::ForceClose(
                "Engine window error",
                "Failed to update engine window because its window ID was not found!");

            return;
        }
        Input* input = Input::GetRegistry().GetContent(pw->GetInputID());
        if (!input)
        {
            KalaWindowCore::ForceClose(
                "Engine window error",
                "Failed to update engine window because its process window input ID was not found!");

            return;
        }
        WindowContext* kgctx = WindowContext::GetRegistry().GetContent(contextID);
        if (!kgctx)
        {
            KalaWindowCore::ForceClose(
                "Engine window error",
                "Failed to update engine window because its context ID was not found!");

            return;
        }

        pw->Update();

        if (!pw->IsIdle()
            && !pw->IsResizing())
        {
            kgctx->Update();
        }

        input->EndFrameUpdate();
    }

    void EngineWindow::Shutdown()
    { 
        ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(windowID);
        if (!pw)
        {
            Log::Print(
                "Failed to shut down engine window because its window ID was not found!",
                "ELYPSO_WINDOW",
                LogType::LOG_ERROR,
                2);

            return;
        }

        pw->CloseWindow();
    }
}