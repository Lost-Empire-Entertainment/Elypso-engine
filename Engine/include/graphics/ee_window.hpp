//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>
#include <vector>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/ee_registry.hpp"
#include "graphics/kw_window.hpp"

namespace ElypsoEngine::Graphics
{
    using KalaHeaders::KalaMath::vec2;

    using ElypsoEngine::Core::ElypsoRegistry;
    using KalaWindow::Graphics::WindowMode;
    using KalaWindow::Graphics::WindowState;

    using std::string_view;
    using std::vector;

    
    class LIB_API EngineWindow
    {
    friend class Scene;

    public:
        static ElypsoRegistry<EngineWindow>& GetRegistry();

        //Create a new window
        static EngineWindow* Initialize(
            string_view windowTitle = "UNASSIGNED TITLE",
            vec2 pos = 800,
            vec2 size = { 800, 600 },
            EngineWindow* parent = nullptr);

        u32 GetID() const;
        //Returns the KalaWindow process window ID
        u32 GetWindowContextID() const;
        //Returns the KalaGraphics context ID
        u32 GetGraphicsContextID() const;

        const vector<u32>& GetSceneIDs() const;

        void Destroy();

        ~EngineWindow();
    private:
        u32 ID{};
        u32 windowContextID{};
        u32 graphicsContextID{};

        vector<u32> sceneIDs{};
    };
}