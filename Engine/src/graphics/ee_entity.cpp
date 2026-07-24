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
    static ElypsoRegistry<Entity> registry{};

    ElypsoRegistry<Entity>& Entity::GetRegistry() { return registry; }

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

        if (!Scene::GetRegistry().createdContent.contains(sceneID))
        {
            Log::Print(
                "Failed to create entity because scene '" + to_string(sceneID) + "' was not found!",
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
                case SubEntityType::S_INVALID:
                {
                    Log::Print(
                        "Failed to create entity because one of the passed subentity types was unassigned!",
                        "EE_ENTITY",
                        LogType::LOG_ERROR,
                        2);

                    return nullptr;
                }
                case SubEntityType::S_MESH:
                {
                    Mesh* mesh = Mesh::GetRegistry().GetContent(s.targetID);
                    if (!mesh)
                    {
                        Log::Print(
                            "Failed to create entity because mesh '" + to_string(s.targetID) + "' was not found!",
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
                case SubEntityType::S_CAMERA:
                {
                    Camera* camera = Camera::GetRegistry().GetContent(s.targetID);
                    if (!camera)
                    {
                        Log::Print(
                            "Failed to create entity because camera '" + to_string(s.targetID) + "' was not found!",
                            "EE_ENTITY",
                            LogType::LOG_ERROR,
                            2);

                        return nullptr;
                    }

                    break;
                }

                default: break;
            }
        }

        entityPtr->subEntities = std::move(subEntities);

        registry.AddContent(newID, std::move(newEntity));

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
        Scene* s = Scene::GetRegistry().GetContent(sceneID);
        if (s)
        {
            auto it = find(s->sceneEntities.begin(), s->sceneEntities.end(), ID);
            if (it != s->sceneEntities.end()) s->sceneEntities.erase(it);
        }

        registry.RemoveContent(ID);
    }

    Entity::~Entity()
    {
        Log::Print(
            "Destroying entity '" + title + "' with ID '" + to_string(ID) + "'.",
            "EE_ENTITY",
            LogType::LOG_INFO);

        for (auto& sub : subEntities)
        {
            switch (sub.type)
            {
            case SubEntityType::S_MESH:
            {
                Mesh* m = Mesh::GetRegistry().GetContent(sub.targetID);
                if (m) m->Destroy();
                else
                {
                    KalaWindowCore::ForceClose(
                        "Entity destruction error",
                        "Failed to destroy entity '" + to_string(ID)
                        + "' because it had an invalid mesh '" + to_string(sub.targetID) + "'!");
                }
            };

            default: break;
            }
        }
    }
}