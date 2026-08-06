//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "graphics/kw_window.hpp"
#include "core/kg_context.hpp"
#include "graphics/kw_vulkan.hpp"
#ifdef __linux__
#include "graphics/kw_window_global.hpp"
#endif

#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "core/ee_core.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::VulkanContext;
using KalaGraphics::Core::GraphicsContext;
using KalaGraphics::Core::GraphicsContextData;
using KalaGraphics::Core::ViewportSize;
#ifdef __linux__
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;
#endif

using ElypsoEngine::Core::EngineCore;

using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace ElypsoEngine::Graphics
{
	static ElypsoRegistry<EngineWindow> registry{};

    static void ShutdownCallback(u32 windowID)
    {
        EngineWindow* enwin = registry.GetContent(windowID);
        if (!enwin)
        {
            Log::Print("Failed to call shutdown callback "
                "because the engine window ID '" + to_string(windowID) + "' was invalid!",
                "EE_WINDOW",
                LogType::LOG_ERROR,
                2);

            return;
        }

        enwin->Destroy();
    }

	ElypsoRegistry<EngineWindow>& EngineWindow::GetRegistry() { return registry; }

    EngineWindow* EngineWindow::Initialize(
        string&& windowTitle,
        vec2 pos,
        vec2 size,
        EngineWindow* parent)
    {
        unique_ptr<EngineWindow> newWindow = make_unique<EngineWindow>();
        EngineWindow* windowPtr = newWindow.get();

        ProcessWindow* pwParent{};
        if (parent)
        {
            pwParent = ProcessWindow::GetRegistry().GetContent(parent->GetWindowContextID());
            if (!pwParent)
            {
                Log::Print(
                    "Failed to assign parent to engine window '" + windowTitle 
                    + "' because the parent engine window '" + to_string(parent->GetID()) 
                    + "' process window '" + to_string(parent->GetWindowContextID()) + "' was invalid!",
                    "EE_WINDOW",
                    LogType::LOG_ERROR,
                    2);

                return nullptr;
            }
        }

        ProcessWindow* pw = ProcessWindow::Initialize(
            string(windowTitle),
            pos,
            size,
            pwParent);

        if (!pw)
        {
            Log::Print(
                "Failed to create process window for engine window '" + windowTitle + "'!",
                "EE_WINDOW",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        u32 windowID = pw->GetID();

        const WindowData& wData = pw->GetWindowData();
#ifdef _WIN32
        GraphicsContextData kgData =
        {
            .windowID = windowID,
            .context_window = wData.window
        };
#else
        const X11GlobalData& data = Window_Global::GetGlobalData();

        GraphicsContextData kgData =
        {
            .windowID = windowID,
            .context_display = data.display,
            .context_window = wData.window
        };
#endif

        VulkanContext* vkctx = VulkanContext::GetRegistry().GetContent(pw->GetGraphicsContextID());
        kgData.context_vk_surface = vkctx->GetSurface();

        //pre-sync to ensure kg gets the highest id
        EngineCore::SyncID();

        GraphicsContext* kgctx = GraphicsContext::InitializeInstance(std::move(kgData));
        if (!kgctx)
        {
            Log::Print(
                "Failed to create graphgics context for engine window '" + windowTitle + "'!",
                "EE_WINDOW",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        pw->SetResizeCallback([kgctx]() 
            {
                if (!kgctx)
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Engine window error",
                        "Failed to call resize callback because graphics context was invalid!");

                    return;
                }

                kgctx->ResizeUpdate();
            });

        //sync to ensure window gets the highest id from kw
        EngineCore::SyncID();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        windowPtr->ID = newID;
        windowPtr->windowContextID = windowID;
        windowPtr->graphicsContextID = kgctx->GetID();

        pw->SetShutdownCallback([newID](){ ShutdownCallback(newID); });

        registry.AddContent(newID, std::move(newWindow));

        Scene* newScene = Scene::Initialize(
            windowTitle + " scene",
            newID);

        newScene->LoadScene();

        Log::Print(
			"Created new window '" + windowTitle 
            + "' with ID '" + to_string(newID) 
            + "' for process window '" + to_string(windowID) + "'!",
			"EE_WINDOW",
			LogType::LOG_SUCCESS);

        return windowPtr;
    }

    u32 EngineWindow::GetID() const { return ID; }
    u32 EngineWindow::GetWindowContextID() const { return windowContextID; }
    u32 EngineWindow::GetGraphicsContextID() const { return graphicsContextID; }

    u32 EngineWindow::GetActiveSceneID() const { return activeSceneID; }
    const vector<u32>& EngineWindow::GetSceneIDs() const { return sceneIDs; }

    void EngineWindow::Destroy() { registry.RemoveContent(ID); }

    EngineWindow::~EngineWindow()
    {
		Log::Print(
			"Destroying engine window '" + to_string(ID) + "'.",
			"EE_WINDOW",
			LogType::LOG_INFO);

        for (auto s : sceneIDs)
        {
            Scene* sc = Scene::GetRegistry().GetContent(s);

            if (!sc)
            {
                Log::Print(
                    "Scene '" + to_string(s) + "' was invalid and couldn't be "
                    "destroyed during the destruction of engine window '" + to_string(ID) + "'!",
                    "EE_WINDOW",
                    LogType::LOG_ERROR,
                    2);

                continue;
            }

            sc->Destroy();
        }

        GraphicsContext* kgctx = GraphicsContext::GetRegistry().GetContent(graphicsContextID);
        if (!kgctx)
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine window error",
                "Failed to destroy engine window '" + to_string(ID)
                + "' because its graphics context '"+ to_string(graphicsContextID) + "' was invalid!");

            return;
        }

        kgctx->Destroy();
    }
}