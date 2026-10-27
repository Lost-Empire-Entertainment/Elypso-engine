//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"
#include "math_utils.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kg_context.hpp"
#include "graphics/kg_hit_test.hpp"
#include "graphics/kg_viewport.hpp"
#include "graphics/kg_shader.hpp"
#include "graphics/kg_camera.hpp"
#include "graphics/kg_material.hpp"
#include "graphics/kg_texture.hpp"
#include "graphics/kg_mesh.hpp"
#include "widgets/kg_text.hpp"

#include "core/core.hpp"
#include "graphics/render.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::vec4;
using KalaHeaders::KalaMath::Transform3D;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::ProcessWindow;
using KalaGraphics::Graphics::GraphicsContext;
using KalaGraphics::Graphics::HitTest;
using KalaGraphics::Graphics::ViewportType;
using KalaGraphics::Graphics::Viewport;
using KalaGraphics::Graphics::Shader;
using KalaGraphics::Graphics::Camera;
using KalaGraphics::Graphics::Texture;

using GraphicsTemplate::Graphics::Render;

using std::string;
using std::to_string;
using std::format;
using std::array;
using std::filesystem::path;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration;

static bool isInitialized{};

static path exePath{};

static EngineWindow* ew1{};
static GraphicsContext* ew1_gctx{};
static Viewport* ew1_gctx_vp1{};
static HitTest* ew1_gctx_ht{};

static ProcessWindow* ew1_pw{};
static Input* ew1_pw_input{};

static Shader* vp1_Shader3D_primary{};
static Shader* vp1_Shader2D_primary{};

static Camera* vp1_Cam3D_primary{};
static Camera* vp1_Cam2D_primary{};

namespace GraphicsTemplate::Core
{
    bool GraphicsTemplateCore::IsInitialized() { return isInitialized; }
    void GraphicsTemplateCore::Initialize()
    {
        if (isInitialized)
        {
            Log::Print(
                "Failed to initialize core because it has already been initialized!",
                "MM_CORE",
                LogType::LOG_WARNING);

            return;
        }

        exePath = KalaWindowCore::GetExePath();

        string err = EngineWindow::GetRegistry().GetContent(0, ew1, false);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get engine window '0'! Reason: " + err);
        }

        err = ProcessWindow::GetRegistry().GetContent(ew1->GetWindowContextID(), ew1_pw);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get process window from engine window '" + to_string(ew1->GetID()) + "'! Reason: " + err);
        }

        ew1_pw->SetMinSize({800, 600});

        err = GraphicsContext::GetRegistry().GetContent(ew1->GetGraphicsContextID(), ew1_gctx);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get graphics context from process window '" + to_string(ew1_pw->GetID()) + "'! Reason: " + err);
        }

        err = HitTest::GetRegistry().GetContent(ew1_gctx->GetHitTestID(), ew1_gctx_ht);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get hit test from graphics context '" + to_string(ew1_gctx->GetID()) + "'! Reason: " + err);
        }

        err = Viewport::GetRegistry().GetContent(ew1_gctx->GetRootViewportID(), ew1_gctx_vp1);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get root viewport from graphics context '" + to_string(ew1_gctx->GetID()) + "'! Reason: " + err);
        }
        ew1_gctx_vp1->SetType(ViewportType::VP_FIT);

        err = Shader::GetRegistry().GetContent(ew1_gctx_vp1->GetShader3DIDs().front(), vp1_Shader3D_primary);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get primary 3D shader from viewport '" + to_string(ew1_gctx_vp1->GetID()) + "'! Reason: " + err);
        }

        err = Shader::GetRegistry().GetContent(ew1_gctx_vp1->GetShader2DIDs().front(), vp1_Shader2D_primary);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get primary 2D shader from viewport '" + to_string(ew1_gctx_vp1->GetID()) + "'! Reason: " + err);
        }

        Texture* rootTex{};
        err = Texture::GetRegistry().GetContent(vp1_Shader3D_primary->GetRootTextureID(), rootTex);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get root texture from shader '" + to_string(vp1_Shader3D_primary->GetID()) + "'! Reason: " + err);
        }
        Render::SetRootTexture(rootTex);

        Texture* fallbackTex{};
        err = Texture::GetRegistry().GetContent(vp1_Shader3D_primary->GetFallbackTextureID(), fallbackTex);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get fallback texture from shader '" + to_string(vp1_Shader3D_primary->GetID()) + "'! Reason: " + err);
        }
        Render::SetFallbackTexture(fallbackTex);

        err = Camera::GetRegistry().GetContent(ew1_gctx_vp1->GetPrimary3DCameraID(), vp1_Cam3D_primary);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get primary 3D camera from viewport '" + to_string(ew1_gctx_vp1->GetID()) + "'! Reason: " + err);
        }
        Transform3D& camTransform = scast<Transform3D&>(vp1_Cam3D_primary->GetTransform());
        camTransform.setpos({ 0.0f, 1.0f, 0.0f });
        camTransform.update();
        vp1_Cam3D_primary->SetSensitivityMultiplier(0.175f);
        vp1_Cam3D_primary->SetSpeedMultiplier(7.5f);

        err = Camera::GetRegistry().GetContent(ew1_gctx_vp1->GetPrimary2DCameraID(), vp1_Cam2D_primary);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get primary 2D camera from viewport '" + to_string(ew1_gctx_vp1->GetID()) + "'! Reason: " + err);
        }

        err = Input::GetRegistry().GetContent(ew1_pw->GetInputID(), ew1_pw_input);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Metal Metropolis core error",
                "Failed to get input from engine windows process window '" + to_string(ew1_pw->GetID()) + "'! Reason: " + err);
        }

        isInitialized = true;
    }

    const path& GraphicsTemplateCore::GetExePath() { return exePath; }
    Input* GraphicsTemplateCore::GetInput() { return ew1_pw_input; }
    GraphicsContext* GraphicsTemplateCore::GetGraphicsContext() { return ew1_gctx; }
    Viewport* GraphicsTemplateCore::GetViewport() { return ew1_gctx_vp1; }
    Shader* GraphicsTemplateCore::Get3DShader() { return vp1_Shader3D_primary; }
    Shader* GraphicsTemplateCore::Get2DShader() { return vp1_Shader2D_primary; }
    Camera* GraphicsTemplateCore::GetCamera() { return vp1_Cam3D_primary; }

    array<string, 4> GraphicsTemplateCore::GetFPS(f64 secondsToWait)
    {
        static f64 cachedFPS = EngineCore::GetAverageFPS();
        static f64 cachedFrameLength = EngineCore::GetAverageFrameLength();
        static f64 cachedOnePercentFPS = EngineCore::GetOnePercentLowFPS();
        static f64 cachedZeroOnePercentFPS = EngineCore::GetZeroPointOnePercentLowFPS();

        static time_point start = steady_clock::now();

        if (steady_clock::now() - start > duration<f64>(secondsToWait))
        {
            cachedFPS = EngineCore::GetAverageFPS();
            cachedFrameLength = EngineCore::GetAverageFrameLength();
            cachedOnePercentFPS = EngineCore::GetOnePercentLowFPS();
            cachedZeroOnePercentFPS = EngineCore::GetZeroPointOnePercentLowFPS();

            start = steady_clock::now();
        }
        
        return
        {
            format("average: {:.2f} fps", cachedFPS),
            format("frame length: {:.2f}ms", cachedFrameLength),
            format("1% low: {:.2f} fps", cachedOnePercentFPS),
            format("0.1% low: {:.2f} fps", cachedZeroOnePercentFPS)
        };
    }
}