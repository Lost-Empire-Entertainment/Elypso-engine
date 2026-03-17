//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <optional>

#include "core_utils.hpp"

struct VkInstance_T;
using VkInstance = VkInstance_T*;

struct VkSurfaceKHR_T;
using VkSurfaceKHR = VkSurfaceKHR_T*;

namespace KalaGraphics::Core
{
    using std::string;
    using std::string_view;
    using std::vector;
    using std::optional;
    using std::nullopt;

    using u8 = uint8_t;
    using u32 = uint32_t;

    enum class RenderTarget : u8
    {
        RT_INVALID = 0u,

        //CPU rendering - falback if no gl or vk context was passed
        RT_SOFTWARE = 1u,

        //OpenGL 3.3 - default renderer unless overridden by graphics features
        RT_OPENGL = 2u,

        //Vulkan 1.3 - prioritized when vk-specific graphics features were chosen
        RT_VULKAN = 3u
    };

    //Graphics features that trigger KalaMake to use Vulkan over OpenGL
    enum class GraphicsFeature : u8
    {
        GF_INVALID = 0u,

        //enforce software rendering even if gl or vk contexts were passed
        GF_FORCE_SOFTWARE = 1u,

        //enforce opengl even if vk features were passed
        GF_FORCE_OPENGL = 2u,

        //enforce vulkan even if capable as opengl
        GF_FORCE_VULKAN = 3u,

        //vk-only compute shaders that enable gpu-driven particle systems,
        //gpu physics, gpu culling, gpu-driven procedual generation and more
        GF_COMPUTE_SHADERS = 4u,

        //vk-only hardware-level raytracing
        GF_RAY_TRACING = 5u,

        //vk-only hardware-level raytracing and pathtracing
        GF_PATH_TRACING = 6u
    };

    struct LIB_API Context
    {
        u32 windowID;

#ifdef _WIN32
        uintptr_t context_window;
#else
        uintptr_t context_display;
        uintptr_t context_window;
#endif

        optional<uintptr_t> context_gl;
        optional<VkSurfaceKHR> context_vk_surface;
    };

    class LIB_API KalaGraphicsCore
    {
    public:
        //Initialize KalaGraphics with your desired features and contexts.
        //If no GL or VK contexts were passed then KalaGraphics falls back to software rendering.
        static bool Initialize(
            const vector<Context>& contexts,
            const optional<vector<GraphicsFeature>> gfxFeatures = nullopt,
            const optional<VkInstance>& vk_instance = nullopt);

        static bool IsInitialized();

        //Regular update - single draw call
        static void Update();

        //Called to trigger resize events - single draw call
        static void ResizeUpdate();

        //Returns the final render target chosen during initialization
        static RenderTarget GetRenderTarget();

        static string GetRenderTargetName();

        //Returns true if this window ID was passed
        static bool IsValidWindowID(u32 windowID);

        //Gives the gl/vk context attached to the window
        static Context GetContext(u32 windowID);

        //Gives the global vk instance
        static VkInstance GetVKInstance();

        //Shuts down KalaGraphics cleanly and frees all resources
        static void Shutdown();

        //Force-closes the application and gives a breakpoint, good for hard stops or bad user errors
		static void ForceClose(
			string_view title,
			string_view reason);
    };
}