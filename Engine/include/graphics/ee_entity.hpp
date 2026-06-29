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

    using u8 = uint8_t;
    using u32 = uint32_t;

    enum class ComponentType : u8
    {
        C_INVALID = 0u,

        //regular camera type
        C_CAMERA = 1u,

        //regular model type
        C_MODEL = 2u,

        //not yet implemented
        C_POINT_LIGHT = 3u,
        //not yet implemented
        C_SPOTLIGHT = 4u,
        //not yet implemented
        C_AREA_LIGHT = 5u,

        //basic widget that can have a texture added to it
        C_UI_RECT = 6u,

        //not yet implemented
        C_PHYSICS_RIGIDBODY = 7u,

        //not yet implemented
        C_AUDIO_LISTENER = 8u,
        //not yet implemented
        C_AUDIO_PLAYER = 9u,
    };

    struct LIB_API Component
    {
        u32 targetID{};
        ComponentType type{};
        bool isEnabled = true;
    };

    class LIB_API Entity
    {
    public:
        static ElypsoRegistry<Entity>& GetRegistry();

        //Initialize a new entity inside a scene.
        //The primary component is a model, light type, UI type or camera,
        //the secondary types are components the primary component uses like a model with a light source.
        static Entity* Initialize(
            string_view title,
            u32 sceneID,
            ComponentType primaryComponent,
            vector<ComponentType> optionalSecondaryComponents = {});

        u32 GetID() const;
        u32 GetSceneID() const;

        string_view GetTitle() const;
        void SetTitle(string_view newTitle);

        void MoveToScene(u32 sceneID);
        void MoveToScene(string_view sceneTitle);

        const Component& GetPrimaryComponent() const;
        //Give this entity a new primary component, the old primary component will be destroyed
        void SetPrimaryComponent(Component newComponent);

        const vector<Component>& GetSecondaryComponents() const;
        //Add a new secondary component to this entity
        void AddSecondaryComponent(Component newComponent);
        //Remove an existing secondary component from this entity, this component will be destroyed
        void RemoveSecondaryComponent(u32 targetID);

        void Destroy();

        ~Entity();
    private:
        u32 ID{};
        u32 sceneID{};

        string title{};

        Component primaryComponent{};
        vector<Component> secondaryComponents{};
    };
}