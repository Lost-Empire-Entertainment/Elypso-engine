//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "graphics/kg_model.hpp"

#include "graphics/ee_entity.hpp"
#include "graphics/ee_scene.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Graphics::Model;

using std::to_string;

namespace ElypsoEngine::Graphics
{
    static ElypsoRegistry<Entity> registry{};

    ElypsoRegistry<Entity>& Entity::GetRegistry() { return registry; }

    Entity* Entity::Initialize(
        string_view title,
        u32 sceneID,
        ComponentType primaryComponent,
        vector<ComponentType> optionalSecondaryComponents)
    {
        return nullptr;
    }

    u32 Entity::GetID() const { return ID; }
    u32 Entity::GetSceneID() const { return sceneID; }

    string_view Entity::GetTitle() const { return title; }
    void Entity::SetTitle(string_view newTitle)
    {

    }

    void Entity::MoveToScene(u32 sceneID)
    {

    }
    void Entity::MoveToScene(string_view sceneTitle)
    {

    }

    const Component& Entity::GetPrimaryComponent() const { return primaryComponent; }
    void Entity::SetPrimaryComponent(Component newComponent)
    {

    }

    const vector<Component>& Entity::GetSecondaryComponents() const { return secondaryComponents; }
    void Entity::AddSecondaryComponent(Component newComponent)
    {

    }
    void Entity::RemoveSecondaryComponent(u32 targetID)
    {

    }

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

        auto destroy_component = [&](const Component& c, bool isPrimary) -> void
            {
                string primaryText = isPrimary ? "primary" : "secondary";

                switch (c.type)
                {
                case ComponentType::C_MODEL:
                {
                    Model* m = Model::GetRegistry().GetContent(c.targetID);
                    if (m) m->Destroy();
                    else
                    {
                        KalaWindowCore::ForceClose(
                            "Entity destruction error",
                            "Failed to destroy entity '" + to_string(ID)
                            + "' because it had an invalid " + primaryText + " component '" + to_string(c.targetID) + "'!");
                    }
                };

                default: break;
                }
            };

        destroy_component(primaryComponent, true);

        for (auto& sec : secondaryComponents)
        {
            destroy_component(sec, false);
        }
    }
}