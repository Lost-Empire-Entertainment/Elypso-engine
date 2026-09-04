//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "graphics/kw_window.hpp"
#if defined(KLIN_ANY)
#include "graphics/kw_window_global.hpp"
#endif
#include "graphics/kw_vulkan.hpp"
#include "core/kg_context.hpp"
#include "core/kg_viewport.hpp"
#include "resources/kg_camera.hpp"

#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "core/ee_core.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::MAX_NAME_LENGTH;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::VulkanContext;
#if defined(KLIN_ANY)
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;
#endif
using KalaGraphics::Core::GraphicsContext;
using KalaGraphics::Core::RootShaderTarget;
using KalaGraphics::Core::Viewport;
using KalaGraphics::Core::GraphicsContextData;
using KalaGraphics::Resources::Camera;
using KalaGraphics::Resources::CameraType;

using ElypsoEngine::Core::EngineCore;

using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace ElypsoEngine::Graphics
{
	static EngineRegistry<EngineWindow> registry{};

	EngineRegistry<EngineWindow>& EngineWindow::GetRegistry() { return registry; }

    static void ShutdownCallback(u32 windowID)
    {
        EngineWindow* ew{};
        string err = registry.GetContent(windowID, ew);
        if (!err.empty())
        {
            Log::Print(
                "Failed to call shutdown callback "
                "because the engine window was invalid! Reason: " + err,
                "EE_WINDOW",
                LogType::LOG_ERROR,
                2);

            return;
        }

        ew->Destroy();
    }

    EngineWindow* EngineWindow::Initialize(
        string&& windowTitle,
        vec2 pos,
        vec2 size,
        EngineWindow* parent)
    {
        if (windowTitle.empty()
            || windowTitle.size() > MAX_NAME_LENGTH)
        {
            Log::Print(
                "Failed to initialize engine window because its name was empty or too long!",
                "EE_WINDOW",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        ProcessWindow* pwParent{};
        if (parent)
        {
            string err = ProcessWindow::GetRegistry().GetContent(parent->GetWindowContextID(), pwParent);
            if (!err.empty())
            {
                Log::Print(
                    "Failed to initialize engine window '" + windowTitle 
                    + "' because its parent engine window '" + to_string(parent->GetID()) 
                    + "' process window was invalid! Reason: " + err,
                    "EE_WINDOW",
                    LogType::LOG_ERROR,
                    2);

                return nullptr;
            }
        }

        //sync for making KW content
        EngineCore::SyncID();

        ProcessWindow* pw = ProcessWindow::Initialize(
            string(windowTitle),
            pos,
            size,
            pwParent);

        if (!pw)
        {
            Log::Print(
                "Failed to initialize engine window '" + windowTitle + "' becase process window creation failed!",
                "EE_WINDOW",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        u32 windowID = pw->GetID();

        const WindowData& wData = pw->GetWindowData();
#if defined(KWIN_ANY)
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

        //sync for making KG content
        EngineCore::SyncID();

        if (!GraphicsContext::IsInitialized()) GraphicsContext::Initialize(VulkanContext::GetInstance());

        VulkanContext* vkctx{};
        string err = VulkanContext::GetRegistry().GetContent(pw->GetGraphicsContextID(), vkctx);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso engine window error",
                "Failed to initialize engine window '" + windowTitle 
                + "' because process window '" + to_string(pw->GetID()) + "' Vulkan context was invalid! Reason: " + err);
        }

        kgData.context_vk_surface = vkctx->GetSurface();

        GraphicsContext* kgctx = GraphicsContext::InitializeInstance(std::move(kgData));
        if (!kgctx)
        {
            Log::Print(
                "Failed to initialize engine window '" + windowTitle 
                + "' because graphics context creation failed!",
                "EE_WINDOW",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        Viewport* vp{};
        err = Viewport::GetRegistry().GetContent(kgctx->GetRootViewportID(), vp);
        if (!err.empty())
        {
            Log::Print(
                "Failed to initialize engine window '" + windowTitle 
                + "' because the graphics context '" + to_string(kgctx->GetID()) + "' root viewport was invalid!",
                "EE_WINDOW",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }
        
        u32 shader3DID = vp->GetRootShaderID(RootShaderTarget::T_UNLIT);
        u32 shader2DID = vp->GetRootShaderID(RootShaderTarget::T_RECT);

        Camera* cam3D = Camera::Initialize(
            shader3DID,
            CameraType::CAM_PERSPECTIVE);

        if (!cam3D)
        {
            KalaWindowCore::ForceClose(
                "Elypso engine window error",
                "Failed to create 3D camera for engine window '" + windowTitle + "'!");
        }

        Camera* cam2D = Camera::Initialize(
            shader2DID,
            CameraType::CAM_ORTHOGRAPHIC);

        if (!cam2D)
        {
            KalaWindowCore::ForceClose(
                "Elypso engine window error",
                "Failed to create 2D camera for engine window '" + windowTitle + "'!");
        }

        //sync for making EE content
        EngineCore::SyncID();

        unique_ptr<EngineWindow> newWindow = make_unique<EngineWindow>();
        EngineWindow* windowPtr = newWindow.get();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        windowPtr->ID = newID;
        windowPtr->windowContextID = windowID;
        windowPtr->graphicsContextID = kgctx->GetID();

        pw->SetResizeCallback([kgctx](bool forceRecreate) 
            {
                if (forceRecreate)
                {
                    bool alreadyRecreated = kgctx->_UpdateInstance();
                    if (!alreadyRecreated) kgctx->_RecreateSwapchain();
                }
                else
                {
                    kgctx->RequestRecreateSwapchain();
                }
            });
        pw->SetShutdownCallback([newID](){ ShutdownCallback(newID); });

        err = registry.AddContent(newID, std::move(newWindow));
        if (!err.empty())
        {
			KalaWindowCore::ForceClose(
				"Elypso engine window error",
				"Failed to initialize window! Reason: " + err);
        }

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

    void EngineWindow::Destroy()
    {
        for (auto s : sceneIDs)
        {
            Scene* sc{};
            string err = Scene::GetRegistry().GetContent(s, sc);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine window error",
                    "Failed to destroy window '" + to_string(ID)
                    + "' because it had an invalid scene! Reason: " + err);

                continue;
            }

            sc->Destroy();
        }

        GraphicsContext* kgctx{};
        string err = GraphicsContext::GetRegistry().GetContent(graphicsContextID, kgctx);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine window error",
                "Failed to destroy engine window '" + to_string(ID)
                + "' because its graphics context was invalid! Reason: " + err);

            return;
        }

        kgctx->Destroy();
        
        err = registry.DestroyContent(ID);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso engine window error",
                "Failed to destroy window '" + to_string(ID) + "'! Reason: " + err);
        }
    }

    EngineWindow::~EngineWindow()
    {
		Log::Print(
			"Destroying engine window '" + to_string(ID) + "'.",
			"EE_WINDOW",
			LogType::LOG_INFO);
    }
}