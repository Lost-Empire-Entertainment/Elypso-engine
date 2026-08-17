//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <chrono>
#include <string>

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
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
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::VulkanContext;
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
using std::vector;
using std::clamp;
using std::milli;
using std::ratio;
using std::format;

using u32 = uint32_t;

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

#ifdef EE_SYSTEM_INFO
    Log::Print("\n======================================================================");

    //print all user system info for now always at the very top

    Log::Print(KalaWindowCore::GetCPUInfoString());
    Log::Print(" ");
    Log::Print(KalaWindowCore::GetGPUInfoString());
    Log::Print(" ");
    Log::Print(KalaWindowCore::GetRAMInfoString(true));
    Log::Print(" ");
    Log::Print(KalaWindowCore::GetOSInfoString());
#endif

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

            //user early logic
            EarlyUpdate();
        };
    auto globalUpdate = []() -> void
        {
            //validate scenes and entities
            ValidateEngineContent();

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
        
    VulkanContext::Initialize(string(appConfig.title));

    KalaGraphicsCore::SetExternalHandler(KalaWindowCore::ForceClose);
    GraphicsContext::Initialize(VulkanContext::GetInstance());

    if (appConfig.pos == 0)
    {
        //TODO: set window pos to monitor center
    }

    EngineWindow::Initialize(
        string(appConfig.title),
        appConfig.pos,
        appConfig.size);

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
            "All engine windows were destroyed, the program has nowhere to draw to, the program must shut down!");
    }

    vector<Entity*> removedEntities{};
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
        if (!s) removedEntities.push_back(e);
    }
    for (Entity* e : removedEntities)
    {
        Log::Print(
            "Destroying entity '" + to_string(e->GetID()) + "' because its scene was invalid!",
            "EE_MAIN",
            LogType::LOG_WARNING);

        Entity::GetRegistry().RemoveContent(e);
    }

    vector<Scene*> removedScenes{};
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
        if (!ew) removedScenes.push_back(s);
    }
    for (Scene* s : removedScenes)
    {
        Log::Print(
            "Destroying scene '" + to_string(s->GetID()) + "' because its window was invalid!",
            "EE_MAIN",
            LogType::LOG_WARNING);

        Scene::GetRegistry().RemoveContent(s);
    }
}