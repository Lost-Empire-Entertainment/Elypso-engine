//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "core/ee_registry.hpp"

namespace ElypsoEngine::Graphics
{
    using ElypsoEngine::Core::EngineRegistry;

    using std::string;
    using std::string_view;
    using std::vector;
    using std::filesystem::path;
    using std::default_delete;

    class LIB_API Scene
    {
    friend class Entity;
    friend struct default_delete<Scene>;
    public:
        KNODISCARD
		static EngineRegistry<Scene>& GetRegistry();

        KNODISCARD
		static Scene* GetActiveScene(u32 windowID);

        //Load a scene by title, this scene is set as active and last loaded scene will be unloaded
        static void LoadScene(string_view title);

        //Create a new scene with the chosen title
        KNODISCARD
		static Scene* Initialize(
            string_view title,
            u32 windowID);

        KNODISCARD
		u32 GetID() const;
        KNODISCARD
		u32 GetWindowID() const;
        KNODISCARD
        vector<u32> GetEntityIDs() const;

        //Add existing sceneless entities or entities from another scene,
        //if any of these entities exist in another scene then
        //they will be removed from there and added to this scene
        void AddEntities(const vector<u32>& entityIDs);

        const string& GetTitle() const;
        void SetTitle(string_view title);

        //Returns true if this scene is currently loaded
        KNODISCARD
		bool IsActiveScene() const;

        //Set this scene as the current active, the last loaded scene will be unloaded.
        //Set keepAlive to true if you want this scenes assets to stay in memory even after the scene is unloaded.
        void LoadScene();

        void Destroy();
    private:
        ~Scene();

        //Unload this scene and disable all its entities
        void Unload();

        u32 ID{};
        u32 windowID{};

        vector<u32> entityIDs{};

        string title{};
    };
}