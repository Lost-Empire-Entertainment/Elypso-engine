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

    enum class SubEntityType : u8
    {
        S_INVALID = 0u,

        //model type from KalaGraphics
        S_MESH = 2u,

        //camera type from KalaGraphics
        S_CAMERA = 1u,
    };

    struct LIB_API SubEntity
    {
        bool isEnabled = true;
        SubEntityType type{};
        u32 targetID{};
    };

    class LIB_API Entity
    {
    public:
        static ElypsoRegistry<Entity>& GetRegistry();

        //Initialize a new entity inside a scene.
        //You must create the subentity in the target library first before assigning its ID here,
        //Elypso Engine will not initialize meshes, cameras and other future types for you
        static Entity* Initialize(
            string&& title,
            u32 sceneID,
            vector<SubEntity>&& subEntities);

        u32 GetID() const;
        u32 GetSceneID() const;

        string_view GetTitle() const;
        bool SetTitle(string&& newTitle);

        bool MoveToScene(u32 sceneID);
        bool MoveToScene(string_view sceneTitle);

        const vector<SubEntity>& GetSubEntities() const;
        bool AddSubEntity(SubEntity&& subEntity);
        //If viaID is true then the subEntity is removed via its ID,
        //otherwise it is removed via its slot in the vector
        bool RemoveSubEntity(
            bool viaID,
            u32 value);

        bool SetParentEntity(u32 entityID);
        void RemoveParentEntity();
        u32 GetParentEntity();

        bool AddChildEntity(u32 entityID);
        //If viaID is true then the child is removed via its ID,
        //otherwise it is removed via its slot in the vector
        bool RemoveChildEntity(
            bool viaID,
            u32 value);
        const vector<u32>& GetChildEntities() const;

        void Destroy();

        ~Entity();
    private:
        u32 ID{};
        u32 sceneID{};

        string title{};

        vector<SubEntity> subEntities{};

        u32 parentEntity{};
        vector<u32> childEntities{};
    };
}