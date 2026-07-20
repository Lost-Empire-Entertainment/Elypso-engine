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
using ElypsoEngine::Core::FixedUpdate;
using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::EngineWindow;
using ElypsoEngine::Graphics::Scene;
using ElypsoEngine::Graphics::Entity;

using std::string;
using std::to_string;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::chrono::time_point;
using std::chrono::steady_clock;
using std::chrono::duration;
using std::vector;
using std::clamp;

using u32 = uint32_t;

static bool isFramerateCapped = true;

//world render framerate target - uncapped value if fps cap is off
static constexpr f64 TARGET_FPS_UNCAPPED = 300.0;
//world render framerate target - capped value if fps cap is on
static constexpr f64 TARGET_FPS_CAPPED = 60.0;

//physics framerate target
static constexpr f64 FIXED_FPS = 60.0;
static constexpr f64 FIXED_DELTA = 1.0 / FIXED_FPS;

//max allowed steps to do in FixedUpdate to catch up with target fps,
//any higher would catch up too agressively and not have any meaningful visual difference
static constexpr u8 MAX_FIXED_STEPS_PER_FRAME = 3;

static f64 deltaTime{};
static f64 frameTime{};
static time_point<steady_clock> lastFrameTime{};

static f64 stepAccumulator{};

static void EngineInit();
static void EnginePreUpdate();
static void EnginePostUpdate();

namespace ElypsoEngine::Core
{
    bool EngineCore::IsFramerateCapped() { return isFramerateCapped; }
    void EngineCore::SetFramerateCapState(bool value)
    {
        isFramerateCapped = value;
    }

    f64 EngineCore::GetDeltaTime() { return deltaTime; }
    f64 EngineCore::GetFrameTime() { return frameTime; }
}

int main()
{
    Log::Print(
        "\n======================================================================"
		"\nSTARTING ENGINE INITIALIZATION"
		"\n======================================================================\n");

    lastFrameTime = steady_clock::now();

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
        auto frameStart = steady_clock::now();
        duration<f64> delta = frameStart - lastFrameTime;
        lastFrameTime = frameStart;

        f64 rawSeconds = delta.count();

        //unscaled, unclamped
        frameTime = rawSeconds;

        //regular deltatime
        deltaTime = clamp(rawSeconds, 0.0, 0.1);

        //input polling, window updates
        EnginePreUpdate();

        stepAccumulator += frameTime;
        u8 fixedStepsThisFrame{};

        while (stepAccumulator >= FIXED_DELTA
               && fixedStepsThisFrame < MAX_FIXED_STEPS_PER_FRAME)
        {

            Log::Print(
                "Calling user-defined fixed update.",
                "EE_MAIN",
                LogType::LOG_DEBUG);

            FixedUpdate();
            stepAccumulator -= FIXED_DELTA;
            fixedStepsThisFrame++;
        }

        Log::Print(
            "Calling user-defined update after " + to_string(fixedStepsThisFrame) + " fixed steps this frame.",
            "EE_MAIN",
            LogType::LOG_DEBUG);

        //user-defined update
        Update();

        //graphics, audio and physics updates
        EnginePostUpdate();

        auto frameEnd = steady_clock::now();
        f64 elapsedSeconds = duration<f64>(frameEnd - frameStart).count();

        f64 targetFrameSeconds = 1 / (isFramerateCapped 
            ? TARGET_FPS_CAPPED 
            : TARGET_FPS_UNCAPPED);

        if (elapsedSeconds < targetFrameSeconds)
        {
            f64 remainingSeconds = targetFrameSeconds - elapsedSeconds;

            Log::Print(
                "Sleeping for '" + to_string(remainingSeconds) + "' remaining seconds.",
                "EE_MAIN",
                LogType::LOG_DEBUG);

            sleep_for(duration<f64>(remainingSeconds));
        }
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