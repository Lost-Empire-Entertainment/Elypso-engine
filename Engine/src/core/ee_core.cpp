//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "core/kg_core.hpp"
#include "core/kp_core.hpp"
#include "core/ka_core.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_render.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::CrashHandler;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Core::KalaGraphicsCore;
using KalaPhysics::Core::KalaPhysicsCore;
using KalaAudio::Core::KalaAudioCore;

using ElypsoEngine::Graphics::Render;

using u32 = uint32_t;

namespace ElypsoEngine::Core
{
    static bool isInitialized{};

    void EngineCore::Initialize(
        string_view programName,
        bool allowGL,
        bool allowVK)
    {
        if (isInitialized)
        {
            Log::Print(
                "Cannot initialize Elypso Engine core structure more than once!",
                "ELYPSO_CORE",
                LogType::LOG_ERROR,
                2);

            return;
        }

        if (programName.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine init error",
                "Program name cannot be empty!");
        }
        if (programName.size() > 50)
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine init error",
                "Program name is too long!");
        }
        
        CrashHandler::Initialize(programName, Shutdown);

        KalaWindowCore::SetUserShutdownCallback(Shutdown);

        Log::Print(
            "Initialized core!",
            "ELYPSO_CORE",
            LogType::LOG_SUCCESS);

        Render::Initialize(allowGL, allowVK);
    }

    bool EngineCore::IsInitialized() { return isInitialized; }

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
            "ELYPSO_CORE",
            LogType::LOG_INFO);

        Render::Shutdown();
    }
}