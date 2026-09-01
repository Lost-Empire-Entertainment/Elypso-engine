//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <chrono>
#include <string>

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"
#include "core/kg_core.hpp"
#include "core/kg_context.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "graphics/ee_entity.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::MAX_NAME_LENGTH;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::ProcessWindow;
using KalaGraphics::Core::KalaGraphicsCore;
using KalaGraphics::Core::GraphicsContext;

using ElypsoEngine::Core::appConfig;
using ElypsoEngine::Core::Init;
using ElypsoEngine::Core::FixedUpdate;
using ElypsoEngine::Core::EarlyUpdate;
using ElypsoEngine::Core::Update;
using ElypsoEngine::Core::LateUpdate;
using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::EngineWindow;
using ElypsoEngine::Graphics::Scene;
using ElypsoEngine::Graphics::Entity;

using std::string;
using std::to_string;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::time_point;
using std::chrono::steady_clock;
using std::chrono::duration;
using std::clamp;
using std::milli;
using std::ratio;
using std::format;

//seconds between displayed smooth fps updates
static constexpr f64 FPS_UPDATE_INTERVAL = 0.5;

//physics framerate target
static constexpr f64 FIXED_FPS = 60.0;
static constexpr f64 FIXED_DELTA = 1.0 / FIXED_FPS;

//max allowed steps to do in FixedUpdate to catch up with target fps,
//any higher would catch up too agressively and not have any meaningful visual difference
static constexpr u8 MAX_FIXED_STEPS_PER_FRAME = 3;

struct FrameLogic
{
    f64 deltaTime{};
    f64 frameTime{};

    f64 rawFPS{};
    f64 finalFPS{};

    f64 stepAccumulator{};

    f64 fpsWindowFrames{};
    f64 fpsWindowAccumulation{};

    time_point<steady_clock> lastFrameTime{};
    time_point<steady_clock, duration<long long, ratio<1, 1000000000>>> frameStart{};

    u8 fixedStepsThisFrame{};
};

static FrameLogic frameLogic{}; 

static void EngineInit();

static void FrameEarlyUpdate();
static void FrameLateUpdate();
static void ValidateEngineContent();

namespace ElypsoEngine::Core
{
    f64 EngineCore::GetDeltaTime() { return frameLogic.deltaTime; }
    f64 EngineCore::GetFrameTime() { return frameLogic.frameTime; }
    f64 EngineCore::GetCurrentFPS() { return frameLogic.finalFPS; }
}

int main()
{
    Log::Print(
        "\n======================================================================"
		"\nSTARTING ENGINE INITIALIZATION"
		"\n======================================================================\n");

    frameLogic.lastFrameTime = steady_clock::now();

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

    auto globalEarlyUpdate = []() -> void
        {
            //start-of-frame timing logic
            FrameEarlyUpdate();

            //kalagraphics update
            GraphicsContext::EarlyUpdate();

            //user early logic
            EarlyUpdate();
        };
    auto globalUpdate = []() -> void
        {
            //validate scenes and entities
            ValidateEngineContent();

            for (EngineWindow* ew : EngineWindow::GetRegistry().GetAllContent())
            {
                GraphicsContext* gctx{};
                string err = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID(), gctx);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso engine main loop error",
                        "Failed to update graphics context input states because "
                        "engine window '" + to_string(ew->GetID()) + "' graphics context was invalid! Reason: " + err);
                }

                ProcessWindow* pw{};
                err = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID(), pw);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso engine main loop error",
                        "Failed to update graphics context input states because "
                        "engine window '" + to_string(ew->GetID()) + "' process window was invalid! Reason: " + err);
                }

                Input* input{};
                err = Input::GetRegistry().GetContent(pw->GetInputID(), input);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso engine main loop error",
                        "Failed to update graphics context input states because "
                        "the process window '" + to_string(ew->GetWindowContextID()) + "' input was invalid! Reason: " + err);
                }

                gctx->SetHeldKeys(input->GetHeldKeys());
                gctx->SetPressedKeys(input->GetPressedKeys());
                gctx->SetReleasedKeys(input->GetReleasedKeys());

                gctx->SetHeldMouseButtons(input->GetHeldMouseButtons());
                gctx->SetPressedMouseButtons(input->GetPressedMouseButtons());
                gctx->SetReleasedMouseButtons(input->GetReleasedMouseButtons());
                gctx->SetDoubleClickedMouseButtons(input->GetDoubleClickedMouseButtons());
                gctx->SetDraggingMouseButtons(input->GetDraggingMouseButtons());

                gctx->SetScrollWheelDelta(input->GetScrollwheelDelta());
            }

            while (frameLogic.stepAccumulator >= FIXED_DELTA
                   && frameLogic.fixedStepsThisFrame < MAX_FIXED_STEPS_PER_FRAME)
            {
                FixedUpdate();

                frameLogic.stepAccumulator -= FIXED_DELTA;
                frameLogic.fixedStepsThisFrame++;
            }

            //user update logic
            Update();

            //kalagraphics update
            GraphicsContext::Update();
        };
    auto globalLateUpdate = []() -> void
        {
            //user late update logic
            LateUpdate();

            //kalagraphics update
            GraphicsContext::LateUpdate();

            //end-of-frame timing logic
            FrameLateUpdate();
        };

    while(true)
    {
        ProcessWindow::Update(
            globalEarlyUpdate,
            globalUpdate,
            globalLateUpdate);
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
        
    Window_Global::SetAppName(string(appConfig.title));
    KalaGraphicsCore::SetExternalHandler(KalaWindowCore::ForceClose);

    if (appConfig.pos == 0)
    {
        //TODO: set window pos to monitor center
    }

    EngineWindow* ew = EngineWindow::Initialize(
        string(appConfig.title),
        appConfig.pos,
        appConfig.size);
    
    if (!ew)
    {
        KalaWindowCore::ForceClose(
            "Elypso engine main loop error",
            "Failed to create primary window!");
    }

    Log::Print(
        "Finished initializing Elypso Engine!",
        "EE_MAIN",
        LogType::LOG_SUCCESS);
}

void FrameEarlyUpdate()
{
    frameLogic.fixedStepsThisFrame = 0;

    frameLogic.frameStart = steady_clock::now();
    duration<f64> delta = frameLogic.frameStart - frameLogic.lastFrameTime;
    frameLogic.lastFrameTime = frameLogic.frameStart;

    f64 rawSeconds = delta.count();

    frameLogic.fpsWindowAccumulation += rawSeconds;
    frameLogic.fpsWindowFrames++;

    frameLogic.rawFPS = (rawSeconds > 0.0) ? (1.0 / rawSeconds) : 0.0;
    f64 displayedFPS{};

    if (frameLogic.fpsWindowAccumulation >= FPS_UPDATE_INTERVAL)
    {
        displayedFPS = scast<f64>(frameLogic.fpsWindowFrames) / frameLogic.fpsWindowAccumulation;
        frameLogic.fpsWindowAccumulation = 0.0;
        frameLogic.fpsWindowFrames = 0;
    }

    frameLogic.finalFPS = (displayedFPS > 0.0) ? displayedFPS : frameLogic.rawFPS;

    //unscaled, unclamped
    frameLogic.frameTime = rawSeconds;

    //regular deltatime
    frameLogic.deltaTime = clamp(rawSeconds, 0.0, 0.1);

    frameLogic.stepAccumulator += frameLogic.frameTime;
}
void FrameLateUpdate()
{
    /*
    auto postWork = steady_clock::now();
    auto elapsed = postWork - frameLogic.frameStart;

    auto frameEnd = steady_clock::now();

    Log::Print(
        format(
            "Smooth framerate: {:.2f} fps | "
            "raw framerate: {:.2f} fps | "
            "frame time: {:.5f} ms | "
            "elapsed frame work: {:.5f} ms",
            frameLogic.finalFPS,
            frameLogic.rawFPS,
            duration<f64, std::milli>(frameEnd - frameLogic.frameStart).count(),
            duration<f64, std::milli>(elapsed).count()),
        "EE_MAIN",
        LogType::LOG_INFO);
    */
}

void ValidateEngineContent()
{
    if (EngineWindow::GetRegistry().GetAllContent().empty())
    {
        KalaWindowCore::ForceClose(
            "Elypso Engine main loop error",
            "All engine windows were destroyed, "
            "the program has nowhere to draw to!");
    }

    for (Entity* e : Entity::GetRegistry().GetAllContent())
    {
        if (!e)
        {
            KalaWindowCore::ForceClose(
                "Engine main loop error",
                "Found dangling entity in runtime content!");
        }
    }

    for (Scene* s : Scene::GetRegistry().GetAllContent())
    {
        if (!s)
        {
            KalaWindowCore::ForceClose(
                "Engine main loop error",
                "Found dangling scene in runtime content!");
        }
    }
}