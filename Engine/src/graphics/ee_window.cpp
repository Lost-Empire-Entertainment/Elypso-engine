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
using KalaGraphics::Core::GraphicsFeature;
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
    static VkInstance vkInstance{};

	static ElypsoRegistry<EngineWindow> registry{};

    static void ShutdownCallback(u32 windowID)
    {
        EngineWindow* enwin = registry.GetContent(windowID);

        if (enwin == nullptr)
        {
            Log::Print("There is no engine window to close because the ID is invalid!",
                "ELYPSO_WINDOW",
                LogType::LOG_ERROR,
                2);

            return;
        }

        Log::Print(
            "Closing Elypso Engine window '" + enwin->GetProcessWindow()->GetTitle() + "'.",
            "ELYPSO_WINDOW",
            LogType::LOG_INFO);

        enwin->GetKGContext()->Shutdown();

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

        windowPtr->window = ProcessWindow::Initialize(windowTitle);
        windowPtr->window->SetClientRectSize(size);
        windowPtr->window->SetPosition(pos);

        u32 windowID = windowPtr->window->GetID();

        windowPtr->input = Input::Initialize(windowID);

        if (!ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_SOFTWARE)
            && !ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_VULKAN)
            && Render::AllowGL()
            && (gfxFeatures.empty()
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_OPENGL)))
        {
            windowPtr->glCont = OpenGL_Context::Initialize(windowID);
        }
        if (!ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_SOFTWARE)
            && !ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_OPENGL)
            && !windowPtr->glCont
            && Render::AllowVK()
            && (gfxFeatures.empty()
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_FORCE_VULKAN)
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_COMPUTE_SHADERS)
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_RAY_TRACING)
            || ContainsValue(gfxFeatures, GraphicsFeature::GF_PATH_TRACING)))
        {
            windowPtr->vkCont = Vulkan_Context::Initialize(windowID);
        }

        const WindowData& wData = windowPtr->window->GetWindowData();
#ifdef _WIN32
        WindowContextData kgCtx =
        {
            .windowID = windowID,
            .isFramebufferDynamic = true,
            .fbSize = FramebufferSize::FB_1920_1080,
            .context_window = wData.window
        };
#else
        const X11GlobalData& data = Window_Global::GetGlobalData();

        WindowContextData kgCtx =
        {
            .windowID = windowID,
            .isFramebufferDynamic = true,
            .fbSize = FramebufferSize::FB_1920_1080,
            .context_display = data.display,
            .context_window = wData.window
        };
#endif

        if (windowPtr->glCont) kgCtx.context_gl = windowPtr->glCont->GetContext();
        if (windowPtr->vkCont) kgCtx.context_vk_surface = windowPtr->vkCont->GetSurface();

        //get kg id up to date with kw id
        EngineCore::SyncID();

        windowPtr->kgCont = WindowContext::Initialize(kgCtx, gfxFeatures);

        windowPtr->window->SetRedrawCallback([windowPtr](){ windowPtr->kgCont->Update(); });
        windowPtr->window->SetResizeCallback([windowPtr](){ windowPtr->kgCont->ResizeUpdate(); });

        windowPtr->window->SetWindowState(state);
        windowPtr->window->SetWindowMode(mode);

        //get engine id up to date with kw id
        EngineCore::SyncID();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        windowPtr->ID = newID;

        windowPtr->window->SetShutdownCallback([newID](){ ShutdownCallback(newID); });

        registry.AddContent(newID, std::move(newWindow));

        Log::Print(
			"Created new window '" + string(windowTitle) + "' with ID '" + to_string(newID) + "'!",
			"ELYPSO_WINDOW",
			LogType::LOG_SUCCESS);

        return windowPtr;
    }

    void EngineWindow::SetVKInstance(VkInstance instance)
    {
        if (!instance)
        {
            Log::Print(
                "Cannot set instance to an empty one!", 
                "ELYPSO_WINDOW",
                LogType::LOG_ERROR,
                2);

            return;
        }

        vkInstance = instance;
    }
    VkInstance EngineWindow::GetVKInstance()
    {
        if (!vkInstance)
        {
            Log::Print(
                "Cannot get VK instance because it is not assigned!", 
                "ELYPSO_WINDOW",
                LogType::LOG_ERROR,
                2);

            return {};
        }

        return vkInstance;
    }

    u32 EngineWindow::GetID() const { return ID; }

    ProcessWindow* EngineWindow::GetProcessWindow() { return window; }
    Input* EngineWindow::GetInput() { return input; }
    WindowContext* EngineWindow::GetKGContext() { return kgCont; }
    OpenGL_Context* EngineWindow::GetGLContext() { return glCont; }
    Vulkan_Context* EngineWindow::GetVKContext() { return vkCont; }

    void EngineWindow::Update()
    {
        if (!window)
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine window update error",
                "Failed to update window because the window is invalid!");

            return;
        }
        if (!kgCont)
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine window update error",
                "Failed to update window '" + string(window->GetTitle()) + "' because the window has no KalaGraphics window context!");
        }

        window->Update();

        if (!window->IsIdle()
            && !window->IsResizing())
        {
            kgCont->Update();
        }

        input->EndFrameUpdate();


    }

    void EngineWindow::Shutdown() { window->CloseWindow(); }
}