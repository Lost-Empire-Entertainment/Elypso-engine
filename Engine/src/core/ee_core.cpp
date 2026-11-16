//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <vector>

#include "log_utils.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window.hpp"
#include "core/kg_core.hpp"
#include "core/kp_core.hpp"
#include "core/ka_core.hpp"
#include "graphics/kw_window.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using ElypsoEngine::Graphics::EngineWindow;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::ProcessWindow;
using KalaGraphics::Core::KalaGraphicsCore;
using KalaPhysics::Core::KalaPhysicsCore;
using KalaAudio::Core::KalaAudioCore;
using KalaWindow::Graphics::ProcessWindow;

using std::vector;
using std::function;

static bool hasCalledEngineShutdown{};
static function<bool()> canCloseCallback{};

namespace ElypsoEngine::Core
{
    void EngineCore::SyncID()
    {
        u32 highest = KalaWindowCore::GetGlobalID();

        //fix individual desync

        if (KalaGraphicsCore::GetGlobalID() > highest) highest = KalaGraphicsCore::GetGlobalID();
        if (KalaPhysicsCore::GetGlobalID() > highest) highest = KalaPhysicsCore::GetGlobalID();
        if (KalaAudioCore::GetGlobalID() > highest) highest = KalaAudioCore::GetGlobalID();

        //bump all to new highest value

        KalaGraphicsCore::SetGlobalID(highest);
        KalaPhysicsCore::SetGlobalID(highest);
        KalaAudioCore::SetGlobalID(highest);

        //and finally sync window id

        KalaWindowCore::SetGlobalID(highest);
    }

    void EngineCore::SetCanCloseCallback(function<bool()> newCanCloseCallback)
    {
        canCloseCallback = newCanCloseCallback;

        for (EngineWindow* ew : EngineWindow::GetRegistry().GetAllContent())
        {
            ProcessWindow* pw{};

            string err = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID(), pw);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine Core error",
                    "Failed to set engine window '" + to_string(ew->GetID()) 
                    + "' process window can close callback because it was invalid! Reason: " + err);
            }

            pw->SetCanCloseCallback(canCloseCallback);
        }
    }

    bool EngineCore::CanClose() { return !canCloseCallback || canCloseCallback(); }

    bool EngineCore::HasCalledEngineShutdown() { return hasCalledEngineShutdown; }
    function<bool()> EngineCore::GetCanCloseCallback() { return canCloseCallback; }

    void EngineCore::Shutdown()
    {
        if (canCloseCallback
            && !canCloseCallback())
        {
			Log::Print(
				"Skipping engine shutdown because shutdown was denied!",
				"EE_CORE",
				LogType::LOG_WARNING);

            return;
        }

        Log::Print(
            "Shutting down Elypso Engine.",
            "EE_CORE",
            LogType::LOG_INFO);

        hasCalledEngineShutdown = true;

        ElypsoEngine::Core::Shutdown();

        vector<ProcessWindow*> windows = ProcessWindow::GetRegistry().GetAllContent();
        for (ProcessWindow* pw : windows) pw->Destroy();
    }
}