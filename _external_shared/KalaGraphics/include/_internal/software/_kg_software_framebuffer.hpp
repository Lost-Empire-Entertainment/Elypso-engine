//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kg_registry.hpp"

namespace KalaGraphics::Internal::Software
{
    using KalaHeaders::KalaMath::vec2;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::vector;
    using u32 = uint32_t;

    class LIB_API Software_Framebuffer
    {
    public:
        static KalaGraphicsRegistry<Software_Framebuffer>& GetRegistry();

        static Software_Framebuffer* Initialize(u32 windowID);

        void CleanScreen(u32 color = 0x00000000);
        void DrawToScreen();

        const u32* GetFrontBuffer() const;
        const u32* GetBackBuffer() const;

        //Swap front and back buffers
        void SwapBuffers();

        //Resize the framebuffer to a new size
        void Resize(vec2 size);
    private:
        u32 windowID{};

        vec2 size{};
        vector<u32> frontBuffer{};
        vector<u32> backBuffer{};
    };
}