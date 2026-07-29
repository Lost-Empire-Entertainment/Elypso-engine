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
using std::chrono::nanoseconds;
using std::chrono::time_point;
using std::chrono::steady_clock;
using std::chrono::duration;
using std::vector;
using std::clamp;
using std::format;
using std::milli;

using u32 = uint32_t;

//world render framerate target - reasonable limit if vsync is off
static constexpr u16 TARGET_FPS_UNCAPPED = 1000;

//seconds between displayed smooth fps updates
constexpr f64 FPS_UPDATE_INTERVAL = 0.5;

//physics framerate target
static constexpr f64 FIXED_FPS = 60.0;
static constexpr f64 FIXED_DELTA = 1.0 / FIXED_FPS;

//max allowed steps to do in FixedUpdate to catch up with target fps,
//any higher would catch up too agressively and not have any meaningful visual difference
static constexpr u8 MAX_FIXED_STEPS_PER_FRAME = 3;

static constexpr nanoseconds frameDuration = nanoseconds{1000000000 / TARGET_FPS_UNCAPPED};

static f64 deltaTime{};
static f64 frameTime{};
static f64 finalFPS{};

static void EngineInit();
static void EnginePreUpdate();
static void EnginePostUpdate();

namespace ElypsoEngine::Core
{
    f64 EngineCore::GetDeltaTime() { return deltaTime; }
    f64 EngineCore::GetFrameTime() { return frameTime; }
    f64 EngineCore::GetCurrentFPS() { return finalFPS; }
}

int main()
{
    Log::Print(
        "\n======================================================================"
		"\nSTARTING ENGINE INITIALIZATION"
		"\n======================================================================\n");

    static f64 stepAccumulator{};

    static f64 fpsWindowFrames{};
    static f64 fpsWindowAccumulation{};

    static time_point<steady_clock> lastFrameTime = steady_clock::now();

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

        fpsWindowAccumulation += rawSeconds;
        fpsWindowFrames++;

        f64 rawFPS = (rawSeconds > 0.0) ? (1.0 / rawSeconds) : 0.0;
        f64 displayedFPS{};

        if (fpsWindowAccumulation >= FPS_UPDATE_INTERVAL)
        {
            displayedFPS = scast<f64>(fpsWindowFrames) / fpsWindowAccumulation;
            fpsWindowAccumulation = 0.0;
            fpsWindowFrames = 0;
        }

        finalFPS = (displayedFPS > 0.0) ? displayedFPS : rawFPS;

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
            /*
            Log::Print(
                "Calling user-defined fixed update.",
                "EE_MAIN",
                LogType::LOG_DEBUG);
            */

            FixedUpdate();
            stepAccumulator -= FIXED_DELTA;
            fixedStepsThisFrame++;
        }

        /*
        Log::Print(
            "Calling user-defined update after " + to_string(fixedStepsThisFrame) + " fixed steps this frame.",
            "EE_MAIN",
            LogType::LOG_DEBUG);
        */

        //user-defined update
        Update();

        //graphics, audio and physics updates
        EnginePostUpdate();

        auto postWork = steady_clock::now();
        auto elapsed = postWork - frameStart;
        auto remaining = frameDuration - elapsed;

        if (remaining > nanoseconds{0})
        {
            /*
            Log::Print(
                format(
                    "Sleeping for {:.5f} remaining milliseconds.",
                    duration<f64, milli>(remaining).count()),
                "EE_MAIN",
                LogType::LOG_INFO);
            */

            sleep_for(remaining);
        }

        /*
        auto frameEnd = steady_clock::now();

        Log::Print(
            format(
                "Smooth framerate: {:.2f} fps | "
                "raw framerate: {:.2f} fps | "
                "frame time: {:.5f} ms | "
                "elapsed frame work: {:.5f} ms | "
                "target frame work: {:.5f} ms",
                finalFPS,
                rawFPS,
                duration<f64, std::milli>(frameEnd - frameStart).count(),
                duration<f64, std::milli>(elapsed).count(),
                duration<f64, std::milli>(frameDuration).count()),
            "EE_MAIN",
            LogType::LOG_INFO);
        */
    }
}

void EngineInit()
{
    if (appConfig.title.empty())
    {
        KalaWindowCore::ForceClose(
            "Elypso Engine main loop error",
            "Failed to initialize Elypso Engine because "
            "program title was empty!");
    }
    if (appConfig.title.size() > MAX_NAME_LENGTH)
    {
        KalaWindowCore::ForceClose(
            "Elypso Engine main loop error",
            "Failed to initialize Elypso Engine because "
            "program title was too long!");
    }
    
    CrashHandler::Initialize(string(appConfig.title));
    
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
        string(appConfig.title),
        appConfig.pos,
        appConfig.size);

    Log::Print(
        "Initialized Elypso Engine!",
        "EE_MAIN",
        LogType::LOG_SUCCESS);
}

void EnginePreUpdate()
{
    if (EngineWindow::GetRegistry().GetAllContent().empty())
    {
        KalaWindowCore::ForceClose(
            "Elypso Engine main loop error",
            "All engine windows were destroyed, the program has nowhere to draw to, the program must shut down!");
    }

#ifdef __linux__
    MessageLoop::Update();
#endif

    for (Entity* e : Entity::GetRegistry().GetAllContent())
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

    for (Scene* s : Scene::GetRegistry().GetAllContent())
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

    for (EngineWindow* ew : EngineWindow::GetRegistry().GetAllContent())
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
    for (EngineWindow* ew : EngineWindow::GetRegistry().GetAllContent())
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