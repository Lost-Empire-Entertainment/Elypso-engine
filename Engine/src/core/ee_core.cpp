//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <vector>

#include "log_utils.hpp"

#include "core/ee_core.hpp"
#include "core/kw_core.hpp"
#include "core/kg_core.hpp"
#include "core/kp_core.hpp"
#include "core/ka_core.hpp"
#include "graphics/kw_window.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Core::KalaGraphicsCore;
using KalaPhysics::Core::KalaPhysicsCore;
using KalaAudio::Core::KalaAudioCore;
using KalaWindow::Graphics::ProcessWindow;

using std::vector;

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

    void EngineCore::Shutdown()
    {
        Log::Print(
            "Shutting down Elypso Engine.",
            "EE_CORE",
            LogType::LOG_INFO);

        vector<ProcessWindow*> pwcopy = ProcessWindow::GetRegistry().runtimeContent;
        for (ProcessWindow* pw : pwcopy)
        {
            pw->Destroy();
        }
    }
}