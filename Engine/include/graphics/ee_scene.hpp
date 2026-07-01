//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "core_utils.hpp"

#include "core/ee_registry.hpp"

namespace ElypsoEngine::Graphics
{
    using ElypsoEngine::Core::ElypsoRegistry;

    using std::string;
    using std::string_view;
    using std::vector;

    using u32 = uint32_t;

    class LIB_API Scene
    {
    public:
        static ElypsoRegistry<Scene>& GetRegistry();

        //Load a scene by title, this scene is set as active and last loaded scene will be unloaded
        static void LoadScene(string_view title);

        static u32 GetActiveSceneID();
        static string GetActiveSceneTitle();

        //Create a new scene with the chosen title
        static Scene* Initialize(
            string_view title,
            u32 windowID);

        u32 GetID() const;
        u32 GetWindowID() const;

        string_view GetTitle() const;
        void SetTitle(string_view title);

        //Returns true if this scene is currently loaded
        bool IsActive() const;

        //Returns true if this scenes entity graphics data will stay in memory after unloading the scene
        bool CanStayAlive() const;
        void SetStayAlive(bool newValue);

        //Set this scene as the current active, the last loaded scene will be unloaded.
        //Set keepAlive to true if you want this scenes assets to stay in memory even after the scene is unloaded.
        void LoadScene();

        //Deletes all entities found within this scene but leaves the scene alive
        void ClearScene();

        void Destroy();

        ~Scene();
    private:
        //Unload this scene and disable all its entities
        void Unload();

        u32 ID{};
        u32 windowID{};

        string title{};

        bool isActive{};
        bool stayAlive{};

        vector<u32> sceneEntities{};
    };
}