//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "resources/kg_mesh.hpp"
#include "resources/kg_camera.hpp"

#include "graphics/ee_entity.hpp"
#include "graphics/ee_scene.hpp"
#include "core/ee_core.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;

using KalaGraphics::Resources::Mesh;
using KalaGraphics::Resources::Camera;

using ElypsoEngine::Core::EngineCore;

using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace ElypsoEngine::Graphics
{
    static EngineRegistry<Entity> registry{};

    EngineRegistry<Entity>& Entity::GetRegistry() { return registry; }

    Entity* Entity::Initialize(
        string&& title,
        u32 sceneID,
        vector<SubEntity>&& subEntities)
    {
        if (subEntities.empty())
        {
            Log::Print(
                "Failed to create entity because no subentities were passed!",
                "EE_ENTITY",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        Scene* sc{};
        string err = Scene::GetRegistry().GetContent(sceneID, sc);
        if (!err.empty())
        {
            Log::Print(
                "Failed to create entity because its scene was invalid! Reason: " + err,
                "EE_ENTITY",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        unique_ptr<Entity> newEntity = make_unique<Entity>();
        Entity* entityPtr = newEntity.get();

        if (!entityPtr->SetTitle(std::move(title)))
        {
            Log::Print(
                "Failed to create entity because title couldn't be set!",
                "EE_ENTITY",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        //sync to ensure entity gets the highest id from kw
        EngineCore::SyncID();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        entityPtr->ID = newID;
        
        bool hasChecked2D{};
        bool is2D{};

        for (const auto& s : subEntities)
        {
            switch (s.type)
            {
            default:
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine core error",
                    "Failed to create entity because of unimplemented type!");
            }
            case SubEntityType::SUBENTITY_MESH:
            {
                Mesh* mesh{};
                string err = Mesh::GetRegistry().GetContent(s.targetID, mesh);
                if (!err.empty())
                {
                    Log::Print(
                        "Failed to create entity because its mesh was invalid! Reason: " + err,
                        "EE_ENTITY",
                        LogType::LOG_ERROR,
                        2);

                    return nullptr;
                }

                if (!hasChecked2D)
                {
                    is2D = mesh->Is2D();
                    hasChecked2D = true;
                }

                if ((is2D
                    && !mesh->Is2D())
                    || (!is2D
                    && mesh->Is2D()))
                {
                    string state = is2D ? "2D" : "3D";

                    Log::Print(
                        "Failed to create entity because mesh '" + to_string(s.targetID) + "' did not match other meshes " +  state + " state!",
                        "EE_ENTITY",
                        LogType::LOG_ERROR,
                        2);

                    return nullptr;
                }

                break;
            }
            case SubEntityType::SUBENTITY_CAMERA:
            {
                Camera* camera{};
                string err = Camera::GetRegistry().GetContent(s.targetID, camera);
                if (!err.empty())
                {
                    Log::Print(
                        "Failed to create entity because its camera was invalid! Reason: " + err,
                        "EE_ENTITY",
                        LogType::LOG_ERROR,
                        2);

                    return nullptr;
                }

                break;
            }
            }
        }

        entityPtr->subEntities = std::move(subEntities);

        err = registry.AddContent(newID, std::move(newEntity));
        if (!err.empty())
        {
			KalaWindowCore::ForceClose(
				"Elypso engine entity error",
				"Failed to initialize entity! Reason: " + err);
        }

        Log::Print(
			"Created new entity '" + to_string(newID) + "'!",
			"KG_ENTITY",
			LogType::LOG_SUCCESS);

        return nullptr;
    }

    u32 Entity::GetID() const { return ID; }
    u32 Entity::GetSceneID() const { return sceneID; }

    string_view Entity::GetTitle() const { return title; }
    bool Entity::SetTitle(string&& newTitle)
    {
        if (newTitle.empty()
            || newTitle.size() > 50)
        {
            Log::Print(
                "Failed to set title because it was empty or too long!",
                "EE_ENTITY",
                LogType::LOG_ERROR,
                2);

            return false;
        }

        title = std::move(newTitle);

        return true;
    }

    bool Entity::MoveToScene(u32 sceneID)
    {
        /*TODO: fill*/
    }
    bool Entity::MoveToScene(string_view sceneTitle)
    {
        /*TODO: fill*/
    }

    const vector<SubEntity>& Entity::GetSubEntities() const { return subEntities; }
    bool Entity::AddSubEntity(SubEntity&& subEntity)
    {
        /*TODO: fill*/

        return false;
    }
    bool Entity::RemoveSubEntity(
        bool viaID,
        u32 value)
    {
        /*TODO: fill*/

        return false;
    }

    bool Entity::SetParentEntity(u32 entityID)
    {
        /*TODO: fill*/

        return false;
    }
    void Entity::RemoveParentEntity()
    {
        /*TODO: fill*/
    }
    u32 Entity::GetParentEntity()
    {
        /*TODO: fill*/

        return {};
    }

    bool Entity::AddChildEntity(u32 entityID)
    {
        /*TODO: fill*/

        return false;
    }
    bool Entity::RemoveChildEntity(
        bool viaID,
        u32 value)
    {
        /*TODO: fill*/

        return false;
    }
    const vector<u32>& Entity::GetChildEntities() const { return childEntities; }

    void Entity::Destroy()
    {
        Scene* sc{};
        string err = Scene::GetRegistry().GetContent(sceneID, sc);
        if (err.empty())
        {
            auto it = find(sc->sceneIDs.begin(), sc->sceneIDs.end(), ID);
            if (it != sc->sceneIDs.end()) sc->sceneIDs.erase(it);
        }

        for (auto& sub : subEntities)
        {
            switch (sub.type)
            {
            default:
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine core error",
                    "Failed to destroy entity '" + to_string(ID)
                    + "' because of unimplemented type!");
            }
            case SubEntityType::SUBENTITY_MESH:
            {
                Mesh* m{};
                string err = Mesh::GetRegistry().GetContent(sub.targetID, m);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Engine core error",
                        "Failed to destroy entity '" + to_string(ID)
                        + "' because it had an invalid mesh! Reason: " + err);
                }
                m->Destroy();
            };
            case SubEntityType::SUBENTITY_CAMERA:
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine core error",
                    "Failed to destroy entity '" + to_string(ID)
                    + "' because camera destruction is unimplemented!");
            }
            }
        }

        err = registry.DestroyContent(ID);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso engine entity error",
                "Failed to destroy entity '" + to_string(ID) + "'! Reason: " + err);
        }
    }

    Entity::~Entity()
    {
        Log::Print(
            "Destroying entity '" + title + "' with ID '" + to_string(ID) + "'.",
            "EE_ENTITY",
            LogType::LOG_INFO);
    }
}