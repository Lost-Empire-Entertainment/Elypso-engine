//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "math_utils.hpp"

#include "core/ee_registry.hpp"

namespace ElypsoEngine::Graphics
{
    using KalaHeaders::KalaMath::Transform3D;
    using KalaHeaders::KalaMath::Transform2D;

    using ElypsoEngine::Core::EngineRegistry;

    using std::string;
    using std::string_view;
    using std::vector;

    enum class ComponentType
    {
        //from KalaGraphics
        MESH_3D,
        //from KalaGraphics
        MESH_2D,

        //from KalaGraphics
        TEXT,
        //from KalaGraphics
        //NOT YET IMPLEMENTED
        CLIPAREA,
        
        //from KalaGraphics,
        //does not support isEnabled
        CAMERA_3D,
        //from KalaGraphics,
        //does not support isEnabled
        CAMERA_2D,

        //from KalaAudio
        //NOT YET IMPLEMENTED
        AUDIO_LISTENER,
        //from KalaAudio
        //NOT YET IMPLEMENTED
        AUDIO_PLAYER,

        //from KalaPhysics
        //NOT YET IMPLEMENTED
        RIGIDBODY,
        //from KalaPhysics
        //NOT YET IMPLEMENTED
        COLLIDER,

        //from KalaServer
        //NOT YET IMPLEMENTED
        SERVER,
        //from KalaServer, only one allowed for entire program
        //NOT YET IMPLEMENTED
        CLIENT
    };

    struct LIB_API Component
    {
        bool isEnabled = true;
        ComponentType type{};
        u32 targetID{};
    };

    class LIB_API Entity
    {
    friend struct EngineRegistry<Entity>;
    public:
        KNODISCARD
		static EngineRegistry<Entity>& GetRegistry();

        //Initialize a new entity inside a scene,
        //assign a KalaGraphics viewport to control which one to use,
        //or leave it blank at let it be auto-assigned
        KNODISCARD
		static Entity* Initialize(
            string_view title,
            u32 sceneID,
            u32 viewportID = 0);

        KNODISCARD
		u32 GetID() const;
        KNODISCARD
		u32 GetSceneID() const;
        //Returns KalaGraphics viewport ID if one has been manually or automatically assigned,
        //returns 0 if no valid one has been manually or automatically added for this entity
        KNODISCARD
        u32 GetViewportID() const;

        const string& GetTitle() const;
		void SetTitle(string_view newTitle);

		void MoveToScene(u32 sceneID);
		void MoveToScene(string_view sceneTitle);

        const vector<Component>& GetComponents() const;
        //Add existing KalaKit component by ID to this entity or leave the ID empty
        //and let KalaKit create it and assign it by type to this entity 
		void AddComponent(Component component);
        //Remove the component and destroy its undelying Kalakit object
		void RemoveComponent(u32 targetValue);

        //Returns true if the object is found and it is enabled,
        //also verifies the underlying KalaKit state for this component
        KNODISCARD
        bool GetComponentEnabledState(u32 targetValue);
        //Sets component enabled state and also sets its underlying KalaKit state
        void SetComponentEnabledState(
            bool newValue,
            u32 targetValue);

        KNODISCARD
		u32 GetParentEntity();
		void SetParentEntity(u32 entityID);
        void RemoveParentEntity();

        KNODISCARD
        const vector<u32>& GetChildEntities() const;
		void AddChildEntity(u32 entityID);
		void RemoveChildEntity(u32 value);

        void Destroy();
    private:
        ~Entity();

        u32 ID{};
        u32 sceneID{};
        u32 viewportID{};

        string title = "UNNAMED_ENTITY";

        vector<Component> components{};

        u32 parentEntity{};
        vector<u32> childEntities{};
    };
}