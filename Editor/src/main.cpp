//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "core/ee_core.hpp"
#include "core/kw_core.hpp"

#include "core/core.hpp"
#include "graphics/render.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using ElypsoEngine::Core::AppConfig;

using KalaWindow::Core::KalaWindowCore;

using ElypsoEditor::Core::ElypsoEditorCore;
using ElypsoEditor::Graphics::Render;

extern const AppConfig ElypsoEngine::Core::appConfig = 
{
    .title = "Elypso Engine",
    .pos = 600
};

void ElypsoEngine::Core::Init()
{
    EngineCore::SetFramerateTimeInterval(1.0f);

    //print all user system info for now always at the very top

    Log::Print(
        KalaWindowCore::GetCPUInfoString() + "\n" +
        KalaWindowCore::GetGPUInfoString() + "\n" +
        KalaWindowCore::GetRAMInfoString(true) + "\n" +
        KalaWindowCore::GetOSInfoString() + "\n");

    ElypsoEditorCore::Initialize();
    Render::Initialize();
}

void ElypsoEngine::Core::EarlyUpdate()
{

}

void ElypsoEngine::Core::FixedUpdate()
{

}

void ElypsoEngine::Core::Update()
{
    Render::Update();
}

void ElypsoEngine::Core::LateUpdate()
{
    
}

void ElypsoEngine::Core::Shutdown()
{

}