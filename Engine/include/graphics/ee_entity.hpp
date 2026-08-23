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
    using ElypsoEngine::Core::EngineRegistry;

    using std::string;
    using std::string_view;
    using std::vector;
    using std::default_delete;

    enum class SubEntityType : u8
    {
        SUBENTITY_MESH    = 0,
        SUBENTITY_CAMERA  = 1
    };

    struct LIB_API SubEntity
    {
        bool isEnabled = true;
        SubEntityType type{};
        u32 targetID{};
    };

    class LIB_API Entity
    {
    friend struct default_delete<Entity>;
    public:
        KNODISCARD
		static EngineRegistry<Entity>& GetRegistry();

        //Initialize a new entity inside a scene.
        //You must create the subentity in the target library first before assigning its ID here,
        //Elypso Engine will not initialize meshes, cameras and other future types for you
        KNODISCARD
		static Entity* Initialize(
            string&& title,
            u32 sceneID,
            vector<SubEntity>&& subEntities);

        KNODISCARD
		u32 GetID() const;
        KNODISCARD
		u32 GetSceneID() const;

        string_view GetTitle() const;
        KNODISCARD
		bool SetTitle(string&& newTitle);

        KNODISCARD
		bool MoveToScene(u32 sceneID);
        KNODISCARD
		bool MoveToScene(string_view sceneTitle);

        const vector<SubEntity>& GetSubEntities() const;
        KNODISCARD
		bool AddSubEntity(SubEntity&& subEntity);
        //If viaID is true then the subEntity is removed via its ID,
        //otherwise it is removed via its slot in the vector
        KNODISCARD
		bool RemoveSubEntity(
            bool viaID,
            u32 value);

        KNODISCARD
		bool SetParentEntity(u32 entityID);
        void RemoveParentEntity();
        KNODISCARD
		u32 GetParentEntity();

        KNODISCARD
		bool AddChildEntity(u32 entityID);
        //If viaID is true then the child is removed via its ID,
        //otherwise it is removed via its slot in the vector
        KNODISCARD
		bool RemoveChildEntity(
            bool viaID,
            u32 value);
        const vector<u32>& GetChildEntities() const;

        void Destroy();
    private:
        ~Entity();

        u32 ID{};
        u32 sceneID{};

        string title{};

        vector<SubEntity> subEntities{};

        u32 parentEntity{};
        vector<u32> childEntities{};
    };
}