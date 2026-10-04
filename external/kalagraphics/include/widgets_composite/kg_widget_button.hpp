//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core_utils.hpp"

#include "core/kg_registry.hpp"

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

namespace KalaGraphics::Graphics
{
    class Viewport;
}

namespace KalaGraphics::CompositeWidgets
{
    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::default_delete;

    class LIB_API Button
    {
    friend class KalaGraphics::Graphics::Viewport;
    friend struct default_delete<Button>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<Button>& GetRegistry();

        KNODISCARD
		static bool IsVerboseLoggingEnabled();
        static void SetVerboseLoggingState(bool state);

        KNODISCARD
        static Button* Initialize(u32 viewportID);

        KNODISCARD
        u32 GetID() const;
        KNODISCARD
        u32 GetShaderID() const;
        KNODISCARD
        u32 GetTextureID() const;
        KNODISCARD
        u32 GetMeshID() const;
        KNODISCARD
        u32 GetTextWidgetID() const;
    
        void Destroy();
    private:
        ~Button();

        void Update(
            VkCommandBuffer buffer,
            f64 deltaTime);

        u32 ID{};
        u32 shaderID{};
        u32 textureID{};
        u32 meshID{};

        u32 textWidgetID{};
    };
}