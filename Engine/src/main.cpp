//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <thread>
#include <chrono>
#include <string>

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_vulkan.hpp"
#ifdef __linux__
#include "core/kw_messageloop_x11.hpp"
#endif
#include "core/kg_core.hpp"
#include "core/kg_context.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "graphics/ee_entity.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::MAX_NAME_LENGTH;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::VulkanContext;
#ifdef __linux__
using KalaWindow::Core::MessageLoop;
#endif
using KalaGraphics::Core::KalaGraphicsCore;
using KalaGraphics::Core::GraphicsContext;

using ElypsoEngine::Core::appConfig;
using ElypsoEngine::Core::Init;
using ElypsoEngine::Core::Update;
using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::EngineWindow;
using ElypsoEngine::Graphics::Scene;
using ElypsoEngine::Graphics::Entity;

using std::string;
using std::to_string;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::vector;

using u32 = uint32_t;

static void EngineInit();
static void EnginePreUpdate();
static void EnginePostUpdate();

int main()
{
    Log::Print(
        "\n======================================================================"
		"\nSTARTING ENGINE INITIALIZATION"
		"\n======================================================================\n");

    //engine-side initialization
    EngineInit();

    Log::Print(
        "\n======================================================================"
		"\nSTARTING USER INITIALIZATION"
		"\n======================================================================\n");

    //user initialization
    Init();

    Log::Print(
        "\n======================================================================"
		"\nSTARTING ENGINE UPDATE"
		"\n======================================================================\n");

    while(true)
    {
        //input polling, window updates
        EnginePreUpdate();

        //user-defined update
        Update();

        //graphics, audio and physics updates
        EnginePostUpdate();

        sleep_for(milliseconds(1));
    }
}

void EngineInit()
{
    if (appConfig.title.empty())
    {
        KalaWindowCore::ForceClose(
            "Elypso Engine init error",
            "Program title cannot be empty!");
    }
    if (appConfig.title.size() > MAX_NAME_LENGTH)
    {
        KalaWindowCore::ForceClose(
            "Elypso Engine init error",
            "Program title cannot be over '" + to_string(MAX_NAME_LENGTH) + "' characters long!");
    }
    
    CrashHandler::Initialize(appConfig.title);
    
    KalaGraphicsCore::SetExternalHandler(KalaWindowCore::ForceClose);

    Window_Global::Initialize();

    VulkanContext::InitializeGlobal();
    GraphicsContext::SetVKInstance(VulkanContext::GetInstance());
    GraphicsContext::InitializeGlobal();

    if (appConfig.pos == 0)
    {
        //todo: set window pos to monitor center
    }

    EngineWindow::Initialize(
        appConfig.title,
        appConfig.pos,
        appConfig.size);

    Log::Print(
        "Initialized Elypso Engine!",
        "EE_MAIN",
        LogType::LOG_SUCCESS);
}

void EnginePreUpdate()
{
    if (EngineWindow::GetRegistry().runtimeContent.empty())
    {
        KalaWindowCore::ForceClose(
            "Engine pre-update error",
            "All engine windows were destroyed, the program has nowhere to draw to, the program must shut down!");
    }

#ifdef __linux__
    MessageLoop::Update();
#endif

    for (Entity* e : Entity::GetRegistry().runtimeContent)
    {
        if (!e)
        {
            Log::Print(
                "Found nullptr entity in runtime content!",
                "EE_MAIN",
                LogType::LOG_ERROR,
                2);

            continue;
        }

        Scene* s = Scene::GetRegistry().GetContent(e->GetSceneID());

        if (!s)
        {
            Log::Print(
                "Destroying entity '" + to_string(e->GetID()) + "' because its scene was invalid!",
                "EE_MAIN",
                LogType::LOG_WARNING);

            e->Destroy();
        }
    }

    for (Scene* s : Scene::GetRegistry().runtimeContent)
    {
        if (!s)
        {
            Log::Print(
                "Found nullptr scene in runtime content!",
                "EE_MAIN",
                LogType::LOG_ERROR,
                2);

            continue;
        }

        EngineWindow* ew = EngineWindow::GetRegistry().GetContent(s->GetWindowID());

        if (!ew)
        {
            Log::Print(
                "Destroying scene '" + to_string(s->GetID()) + "' because its window was invalid!",
                "EE_MAIN",
                LogType::LOG_WARNING);

            s->Destroy();
        }
    }

    for (EngineWindow* ew : EngineWindow::GetRegistry().runtimeContent)
    {
        if (!ew)
        {
            Log::Print(
                "Failed to run engine window pre-update because it was invalid!",
                "EE_MAIN",
                LogType::LOG_ERROR,
                2);

            continue;
        } 

        ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID());
        if (!pw)
        {
            Log::Print(
                "Failed to run engine window '" + to_string(ew->GetID()) + "' pre-update because its window ID '" + to_string(ew->GetWindowContextID()) + "' was not found!",
                "EE_MAIN",
                LogType::LOG_ERROR,
                2);

            continue;
        }

        pw->Update();
    }
}

void EnginePostUpdate()
{
    for (EngineWindow* ew : EngineWindow::GetRegistry().runtimeContent)
    {
        if (!ew)
        {
            Log::Print(
                "Failed to run engine window post-update because one of its windows is invalid!",
                "EE_MAIN",
                LogType::LOG_ERROR,
                2);

            continue;
        } 

        u32 ewID = ew->GetID();

        ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID());
        if (!pw)
        {
            Log::Print(
                "Failed to run engine window '" + to_string(ewID) + "' post-update because its window ID '" + to_string(ew->GetWindowContextID()) + "' was not found!",
                "EE_MAIN",
                LogType::LOG_ERROR,
                2);

            continue;
        }
    
        Input* input = Input::GetRegistry().GetContent(pw->GetInputID());
        if (!input)
        {
            Log::Print(
                "Failed to run engine window '" + to_string(ewID) + "' post-update because its input ID '" + to_string(pw->GetInputID()) + "' was not found!",
                "EE_MAIN",
                LogType::LOG_ERROR,
                2);

            continue;
        }

        GraphicsContext* kgctx = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID());
        if (!kgctx)
        {
            Log::Print(
                "Failed to run engine window '" + to_string(ewID) + "' post-update because its context ID '" + to_string(ew->GetGraphicsContextID()) + "' was not found!",
                "EE_MAIN",
                LogType::LOG_ERROR,
                2);

            continue;
        }

        if (!pw->IsIdle()) kgctx->Update();

        input->EndFrameUpdate();
    }
}