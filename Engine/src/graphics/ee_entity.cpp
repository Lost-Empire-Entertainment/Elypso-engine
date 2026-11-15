//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "core_utils.hpp"
#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "graphics/kg_mesh.hpp"
#include "graphics/kg_camera.hpp"
#include "graphics/kg_shader.hpp"
#include "graphics/kg_context.hpp"
#include "graphics/kg_viewport.hpp"

#include "graphics/ee_entity.hpp"
#include "graphics/ee_scene.hpp"
#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"

using KalaHeaders::KalaCore::ContainsValue;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;

using KalaGraphics::Graphics::Mesh;
using KalaGraphics::Graphics::CameraType;
using KalaGraphics::Graphics::Camera;
using KalaGraphics::Graphics::Shader;
using KalaGraphics::Graphics::GraphicsContext;
using KalaGraphics::Graphics::Viewport;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::EngineWindow;

using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace ElypsoEngine::Graphics
{
    static EngineRegistry<Entity> registry{};

    EngineRegistry<Entity>& Entity::GetRegistry() { return registry; }

    Entity* Entity::Initialize(
        string_view title,
        u32 sceneID,
        u32 viewportID)
    {
        Scene* s{};
        string err = Scene::GetRegistry().GetContent(sceneID, s);
        if (!err.empty())
        {
            Log::Print(
                "Failed to create entity because its scene was invalid! Reason: " + err,
                "EE_ENTITY",
                LogType::WARNING);

            return nullptr;
        }

        if (viewportID != 0)
        {
            Viewport* vp{};
            err = Viewport::GetRegistry().GetContent(viewportID, vp);
            if (!err.empty())
            {
                Log::Print(
                    "Failed to create entity because its viewport was invalid! Reason: " + err,
                    "EE_ENTITY",
                    LogType::WARNING);

                return nullptr;
            }

            EngineWindow* ew{};
            err = EngineWindow::GetRegistry().GetContent(s->GetWindowID(), ew);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine entity error",
                    "Failed to create entity because scene '" 
                    + to_string(sceneID) + "' engine window was invalid! Reason: " + err);
            }

            GraphicsContext* gctx{};
            err = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID(), gctx);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine entity error",
                    "Failed to create entity because engine window '" 
                    + to_string(ew->GetID()) + "' graphics context was invalid! Reason: " + err);
            }

            if (gctx->GetRootViewportID() != viewportID
                && !ContainsValue(gctx->GetExtraViewportIDs(), viewportID))
            {
                Log::Print(
                    "Failed to create entity because viewport '" 
                    + to_string(viewportID) + "' is not valid for the current scene!",
                    "EE_ENTITY",
                    LogType::WARNING);

                return nullptr;
            }
        }

        for (const u32 eid : s->GetEntityIDs())
        {
            Entity* e{};
            string err = registry.GetContent(eid, e);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso engine entity error",
                    "Failed to initialize entity because scene entity was invalid! Reason: " + err);
            }

            if (e->GetTitle() == title)
            {
                Log::Print(
                    "One or more entities in scene '" + to_string(sceneID) 
                    + "' already share the same title '" + string(title) + "', this is discouraged.",
                    "EE_ENTITY",
                    LogType::WARNING);

                break;
            }
        }

        unique_ptr<Entity> newEntity = make_unique<Entity>();
        Entity* entityPtr = newEntity.get();

        entityPtr->SetTitle(title);

        //sync to ensure entity gets the highest id from kw
        EngineCore::SyncID();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        entityPtr->ID = newID;
        entityPtr->sceneID = sceneID;
        entityPtr->viewportID = viewportID;

        s->entityIDs.push_back(newID);

        err = registry.AddContent(newID, std::move(newEntity));
        if (!err.empty())
        {
			KalaWindowCore::ForceClose(
				"Elypso engine entity error",
				"Failed to initialize entity! Reason: " + err);
        }

        Log::Print(
			"Created new entity '" + to_string(newID) + "'!",
			"EE_ENTITY",
			LogType::SUCCESS);

        return entityPtr;
    }

    u32 Entity::GetID() const { return ID; }
    u32 Entity::GetSceneID() const { return sceneID; }
    u32 Entity::GetViewportID() const
    {
        if (viewportID == 0)
        {
            Log::Print(
                "Failed to get viewport ID because none has been manually or automatically assigned yet!",
                "EE_ENTITY",
                LogType::WARNING);
        }
        
        return viewportID;
    }

    const string& Entity::GetTitle() const { return title; }
    void Entity::SetTitle(string_view newTitle)
    {
        if (newTitle.empty()
            || newTitle.size() > 50)
        {
            Log::Print(
                "Failed to set title because it was empty or too long!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        title = newTitle;
    }

    void Entity::MoveToScene(u32 targetSceneID)
    {
        Scene* thisScene{};
        string err = Scene::GetRegistry().GetContent(sceneID, thisScene);
        if (!err.empty())
        {
			KalaWindowCore::ForceClose(
				"Elypso engine entity error",
				"Failed to move entity '" + to_string(ID) 
                + "' to target scene because entity current scene is invalid! Reason: " + err);
        }

        if (thisScene->ID == targetSceneID)
        {
            Log::Print(
                "Failed to move entity '" + to_string(ID) 
                + "' to target scene '" + to_string(targetSceneID) + "' because it is already the same!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        Scene* targetScene{};
        err = Scene::GetRegistry().GetContent(targetSceneID, targetScene);
        if (!err.empty())
        {
            Log::Print(
                "Failed to move entity '" + to_string(ID) 
                + "' to target scene because it is invalid! Reason: " + err,
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        erase(thisScene->entityIDs, ID);
        targetScene->entityIDs.push_back(ID);
        sceneID = targetSceneID;

        Log::Print(
            "Moved entity '" + to_string(ID) + "'"
            " to target scene '" + to_string(targetSceneID) + "'!",
            "EE_ENTITY",
            LogType::SUCCESS);
    }
    void Entity::MoveToScene(string_view sceneTitle)
    {
        Scene* thisScene{};
        string err = Scene::GetRegistry().GetContent(sceneID, thisScene);
        if (!err.empty())
        {
			KalaWindowCore::ForceClose(
				"Elypso engine entity error",
				"Failed to move entity '" + to_string(ID) 
                + "' to target scene because entity current scene is invalid! Reason: " + err);
        }

        if (thisScene->title == sceneTitle)
        {
            Log::Print(
                "Failed to move entity '" + to_string(ID) 
                + "' to target scene '" + string(sceneTitle) + "' because it is already the same!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        Scene* targetScene{};
        for (Scene* s : Scene::GetRegistry().GetAllContent())
        {
            if (s->title == sceneTitle)
            {
                targetScene = s;
                break;
            }
        }

        if (!targetScene)
        {
            Log::Print(
                "Failed to move entity '" + to_string(ID) 
                + "' to target scene because no scene exists with title '" + string(sceneTitle) + "'!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        erase(thisScene->entityIDs, ID);
        targetScene->entityIDs.push_back(ID);
        sceneID = targetScene->ID;

        Log::Print(
            "Moved entity '" + to_string(ID) + "'"
            " to target scene '" + to_string(targetScene->ID) + "'!",
            "EE_ENTITY",
            LogType::SUCCESS);
    }

    const vector<Component>& Entity::GetComponents() const { return components; }
    void Entity::AddComponent(Component component)
    {
        //prevent multiple clients
        if (component.type == ComponentType::CLIENT)
        {
            if (component.targetID != 0)
            {
                Log::Print(
                    "Failed to add component '" + to_string(component.targetID) + "' to entity '" + to_string(ID) 
                    + "' because it is a client component and it doesn't use ID!",
                    "EE_ENTITY",
                    LogType::WARNING);

                return;
            }
            for (const Entity* e : registry.GetAllContent())
            {
                for (const Component& c : e->components)
                {
                    if (c.type == ComponentType::CLIENT)
                    {
                        Log::Print(
                            "Failed to add client component to entity '" + to_string(ID) 
                            + "' because entity '" + to_string(e->ID) + "' already has a client component!",
                            "EE_ENTITY",
                            LogType::WARNING);

                        return;
                    }
                }
            }
        }

        //make sure component isnt already used in another entity
        if (component.targetID != 0)
        {
            for (const Entity* e : registry.GetAllContent())
            {
                for (const Component& c : e->components)
                {
                    if (c.targetID == component.targetID)
                    {
                        Log::Print(
                            "Failed to add component '" + to_string(component.targetID) + "' to entity '" + to_string(ID) 
                            + "' because entity '" + to_string(e->ID) + "' already has that component!",
                            "EE_ENTITY",
                            LogType::WARNING);

                        return;
                    }
                }
            }
        }

        //check if component type is implemented and if if non-empty component ID is valid
        switch (component.type)
        {
        default:
        {
            Log::Print(
                "Failed to add component to entity '" + to_string(ID) 
                + "' because the component type '" + to_string(scast<u8>(component.type)) + "' is unimplemented!",
                "EE_ENTITY",
                LogType::WARNING);
            return;
        }
        case ComponentType::MESH_3D:
        case ComponentType::MESH_2D:
        {
            if (component.targetID != 0)
            {
                Mesh* mesh{};
                string err = Mesh::GetRegistry().GetContent(component.targetID, mesh);
                if (!err.empty())
                {
                    Log::Print(
                        "Failed to add component '" + to_string(component.targetID) + "' to entity '" 
                        + to_string(ID) + "' because the component was not found in mesh registry!",
                        "EE_ENTITY",
                        LogType::WARNING);

                    return;
                }
                if (component.isEnabled == false) mesh->SetVisibleState(false);
            }
            break;
        }
        case ComponentType::CAMERA_3D:
        case ComponentType::CAMERA_2D:
        {
            if (component.isEnabled == false)
            {
                component.isEnabled = true;

                Log::Print(
                    "A camera enabled state can never be set to false, so a camera component cannot be initialized as false. "
                    "Overrode camera component enabled state for entity '" + to_string(ID) + "' to true.",
                    "EE_ENTITY",
                    LogType::WARNING);
            }

            if (component.targetID != 0)
            {
                Camera* cam{};
                string err = Camera::GetRegistry().GetContent(component.targetID, cam);
                if (!err.empty())
                {
                    Log::Print(
                        "Failed to add component '" + to_string(component.targetID) + "' to entity '" 
                        + to_string(ID) + "' because the component was not found in camera registry!",
                        "EE_ENTITY",
                        LogType::WARNING);

                    return;
                }
            }
            break;
        }
        }

        auto is_3d_type = [component]() -> bool
            {
                return 
                    component.type == ComponentType::MESH_3D
                    || component.type == ComponentType::CAMERA_3D;
            };
        auto is_2d_type = [component]() -> bool
            {
                return 
                    component.type == ComponentType::MESH_2D
                    || component.type == ComponentType::CAMERA_2D
                    || component.type == ComponentType::TEXT
                    || component.type == ComponentType::CLIPAREA;
            };

        auto has_3d_types = [&]() -> bool
            {
                for (const Component& c : components)
                {
                    if (c.type == ComponentType::MESH_3D
                        || c.type == ComponentType::CAMERA_3D)
                    {
                        return true;
                    }
                }

                return false;
            };
        auto has_2d_types = [&]() -> bool
            {
                for (const Component& c : components)
                {
                    if (c.type == ComponentType::MESH_2D
                        || c.type == ComponentType::CAMERA_2D
                        || c.type == ComponentType::TEXT
                        || c.type == ComponentType::CLIPAREA)
                    {
                        return true;
                    }
                }

                return false;
            };

        //ensure 2D and 3D KalaGraphics objects dont get mixed up
        if (is_3d_type()
            && has_2d_types())
        {
            Log::Print(
                "Failed to add component '" + to_string(component.targetID) + "' to entity '" 
                + to_string(ID) + "' because this is a 3D component but this entity already has 2D components!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }
        else if (is_2d_type()
            && has_3d_types())
        {
            Log::Print(
                "Failed to add component '" + to_string(component.targetID) + "' to entity '" 
                + to_string(ID) + "' because this is a 2D component but this entity already has 3D components!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        //create new component
        if (component.targetID == 0
            && component.type != ComponentType::CLIENT)
        {
            auto get_shader = [&](
                u32 vpID,
                bool is3D) -> u32
                {
                    Scene* s{};
                    string err = Scene::GetRegistry().GetContent(sceneID, s);
                    if (!err.empty())
                    {
                        KalaWindowCore::ForceClose(
                            "Elypso Engine entity error",
                            "Failed to create new component for entity '" + to_string(ID) 
                            + "' because scene was invalid! Reason: " + err);
                    }

                    EngineWindow* ew{};
                    err = EngineWindow::GetRegistry().GetContent(s->GetWindowID(), ew);
                    if (!err.empty())
                    {
                        KalaWindowCore::ForceClose(
                            "Elypso Engine entity error",
                            "Failed to create new component for entity '" + to_string(ID) 
                            + "' because scene '" + to_string(sceneID) + "' engine window was invalid! Reason: " + err);
                    }

                    GraphicsContext* gctx{};
                    err = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID(), gctx);
                    if (!err.empty())
                    {
                        KalaWindowCore::ForceClose(
                            "Elypso Engine entity error",
                            "Failed to create new component for entity '" + to_string(ID) 
                            + "' because engine window '" + to_string(ew->GetID()) + "' graphics context was invalid! Reason: " + err);
                    }

                    Viewport* vp{};
                    if (vpID == 0)
                    {
                        err = Viewport::GetRegistry().GetContent(gctx->GetRootViewportID(), vp);
                        if (!err.empty())
                        {
                            KalaWindowCore::ForceClose(
                                "Elypso Engine entity error",
                                "Failed to create new component for entity '" + to_string(ID) 
                                + "' because graphics context '" + to_string(gctx->GetID()) + "' viewport was invalid! Reason: " + err);
                        }
                    }
                    else
                    {
                        if (gctx->GetRootViewportID() != vpID
                            && !ContainsValue(gctx->GetExtraViewportIDs(), vpID))
                        {
                            Log::Print(
                                "Failed to create new component for entity '" + to_string(ID) 
                                + "' because viewport '" + to_string(viewportID) + "' is not valid for the current scene!",
                                "EE_ENTITY",
                                LogType::WARNING);

                            return 0;
                        }

                        if (vpID == gctx->GetRootViewportID())
                        {
                            string err = Viewport::GetRegistry().GetContent(vpID, vp);
                            if (!err.empty())
                            {
                                KalaWindowCore::ForceClose(
                                    "Elypso Engine entity error",
                                    "Failed to create new component for entity '" 
                                    + to_string(ID) + "' because root viewport was invalid! Reason: " + err);
                            }
                        }
                        else
                        {
                            for (const u32 extraVPID : gctx->GetExtraViewportIDs())
                            {
                                if (extraVPID != vpID) continue;

                                string err = Viewport::GetRegistry().GetContent(extraVPID, vp);
                                if (!err.empty())
                                {
                                    KalaWindowCore::ForceClose(
                                        "Elypso Engine entity error",
                                        "Failed to create new component for entity '" 
                                        + to_string(ID) + "' because extra viewport was invalid! Reason: " + err);
                                }

                                break;
                            }
                        }
                    }

                    return is3D 
                        ? vp->GetShader3DIDs().front()
                        : vp->GetShader2DIDs().front();
                };

            switch (component.type)
            {
            default:
            case ComponentType::MESH_3D:
            {
                u32 shaderID = get_shader(viewportID, true);
                if (shaderID == 0) return;

                EngineCore::SyncID();
                Mesh* mesh = Mesh::Initialize(shaderID);
                
                if (!mesh) return;
                component.targetID = mesh->GetID();

                break;
            }
            case ComponentType::MESH_2D:
            {
                u32 shaderID = get_shader(viewportID, false);
                if (shaderID == 0) return;

                EngineCore::SyncID();
                Mesh* mesh = Mesh::Initialize(shaderID);

                if (!mesh) return;
                component.targetID = mesh->GetID();

                break;
            }
            case ComponentType::CAMERA_3D:
            {
                u32 shaderID = get_shader(viewportID, true);
                if (shaderID == 0) return;

                EngineCore::SyncID();
                Camera* cam = Camera::Initialize(shaderID);

                if (!cam) return;
                component.targetID = cam->GetID();

                break;
            }
            case ComponentType::CAMERA_2D:
            {
                u32 shaderID = get_shader(viewportID, false);
                if (shaderID == 0) return;

                EngineCore::SyncID();
                Camera* cam = Camera::Initialize(
                    shaderID,
                    CameraType::ORTHOGRAPHIC); 

                if (!cam) return;
                component.targetID = cam->GetID();

                break;
            }
            }
        }

        components.push_back(component);

        Log::Print(
            "Added new component '" + to_string(component.targetID) + "' to entity '" + to_string(ID) + "'!",
            "EE_ENTITY",
            LogType::SUCCESS);
    }
    void Entity::RemoveComponent(u32 value)
    {
        auto it = components.end();
        it = find_if(components.begin(), components.end(),
            [&](const Component& c)
            {
                return c.targetID == value;
            });

        if (it == components.end())
        {
            Log::Print(
                "Failed to remove component '" + to_string(value) + "' from entity '" 
                + to_string(ID) + "' because the component was invalid!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        Component component = *it;
        components.erase(it);

        switch (component.type)
        {
        default:
        case ComponentType::MESH_3D:
        case ComponentType::MESH_2D:
        {
            Mesh* m{};
            string _ = Mesh::GetRegistry().GetContent(component.targetID, m);

            if (!m)
            {
                Log::Print(
                    "Failed to destroy mesh '" + to_string(component.targetID) 
                    + "' for entity '" + to_string(ID) + "' probably because "
                    "the mesh was already destroyed! Components should not be manually destroyed.",
                    "EE_ENTITY",
                    LogType::WARNING);

                return;
            }

            m->Destroy();

            break;
        }
        case ComponentType::CAMERA_3D:
        case ComponentType::CAMERA_2D:
        {
            Camera* c{};
            string _ = Camera::GetRegistry().GetContent(component.targetID, c);

            if (!c)
            {
                Log::Print(
                    "Failed to destroy camera '" + to_string(component.targetID) 
                    + "' for entity '" + to_string(ID) + "' probably because "
                    "the camera was already destroyed! Components should not be manually destroyed.",
                    "EE_ENTITY",
                    LogType::WARNING);

                return;
            }

            c->Destroy();

            break;
        }
        }

        Log::Print(
            "Removed component '" + to_string(value) 
            + "' from entity '" + to_string(ID) + "'!",
            "EE_ENTITY",
            LogType::SUCCESS);
    }

    bool Entity::GetComponentEnabledState(u32 targetValue)
    {
        Component* component{};
        for (Component& c : components)
        {
            if (c.targetID == targetValue)
            {
                component = &c;
                break;
            }
        }

        if (component->targetID == 0)
        {
            Log::Print(
                "Failed to get component '" + to_string(targetValue) + "' enabled state for entity '" 
                + to_string(ID) + "' because the component was not found on this entity!",
                "EE_ENTITY",
                LogType::WARNING);

            return false;
        }

        switch (component->type)
        {
        default:
        case ComponentType::MESH_3D:
        case ComponentType::MESH_2D:
        {
            Mesh* m{};
            string err = Mesh::GetRegistry().GetContent(targetValue, m);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine entity error",
                    "Failed to get entity '" + to_string(ID) 
                    + "' component enabled state because it was invalid! Reason: " + err);
            }

            if (component->isEnabled != m->IsVisible())
            {
                component->isEnabled = m->IsVisible();
                string enabledStr = component->isEnabled ? "true" : "false";

                Log::Print(
                    "Mesh '" + to_string(component->targetID) 
                    + "' visible state did not match the components enabled state in entity '" + to_string(ID) 
                    + "'! Updating component enabled state to new value '" + enabledStr + "'.",
                    "EE_ENTITY",
                    LogType::WARNING);
            }

            break;
        }
        case ComponentType::CAMERA_3D:
        case ComponentType::CAMERA_2D:
        {
            Log::Print(
                "Cameras are always enabled, so GetComponentEnabledState always returns true.",
                "EE_ENTITY",
                LogType::WARNING);

            break;
        }
        }

        return component->isEnabled;
    }
    void Entity::SetComponentEnabledState(
        bool newValue, 
        u32 targetValue)
    {
        Component* component{};
        for (Component& c : components)
        {
            if (c.targetID == targetValue)
            {
                component = &c;
                break;
            }
        }

        if (!component)
        {
            Log::Print(
                "Failed to set component '" + to_string(targetValue) + "' enabled state for entity '" 
                + to_string(ID) + "' because the component was not found on this entity!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        bool isAlreadySame{};
        bool bothDoNotMatch{};

        switch (component->type)
        {
        default:
        case ComponentType::MESH_3D:
        case ComponentType::MESH_2D:
        {
            Mesh* m{};
            string err = Mesh::GetRegistry().GetContent(targetValue, m);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine entity error",
                    "Failed to get entity '" + to_string(ID) 
                    + "' component enabled state because it was invalid! Reason: " + err);
            }

            if (m->IsVisible() == newValue)
            {
                isAlreadySame = true;
            }
            if (m->IsVisible() != component->isEnabled)
            {
                bothDoNotMatch = true;
            }

            m->SetVisibleState(newValue);
            component->isEnabled = newValue;

            break;
        }
        case ComponentType::CAMERA_3D:
        case ComponentType::CAMERA_2D:
        {
            Log::Print(
                "Failed to set camera component enabled state because camera enabled state can only be true!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }
        }

        string enabledStr = component->isEnabled ? "true" : "false";

        if (bothDoNotMatch)
        {
            Log::Print(
                "Entity '" + to_string(ID) + "' component '" 
                + to_string(component->targetID) + "' and its underlying KalaKit object state did not match! "
                "It is discouraged to manually update it outside of Elypso Engine.",
                "EE_ENTITY",
                LogType::WARNING);
        }
        if (isAlreadySame)
        {
            Log::Print(
                "Failed to set entity '" + to_string(ID) + "' component '" 
                + to_string(component->targetID) + "' enabled state to '" + enabledStr + "'!",
                "EE_ENTITY",
                LogType::WARNING);

            return;
        }

        Log::Print(
            "Set entity '" + to_string(ID) + "' component '" 
            + to_string(component->targetID) + "' enabled state to '" + enabledStr + "'!",
            "EE_ENTITY",
            LogType::SUCCESS);
    }

    u32 Entity::GetParentEntity()
    {
        /*TODO: fill*/

        return {};
    }

    void Entity::SetParentEntity(u32 entityID)
    {
        /*TODO: fill*/
    }
    void Entity::RemoveParentEntity()
    {
        /*TODO: fill*/
    }

    const vector<u32>& Entity::GetChildEntities() const { return childEntities; }
    void Entity::AddChildEntity(u32 entityID)
    {
        /*TODO: fill*/
    }
    void Entity::RemoveChildEntity(u32 value)
    {
        /*TODO: fill*/
    }

    void Entity::Destroy()
    {
        Scene* s{};
        string err = Scene::GetRegistry().GetContent(sceneID, s);
        if (err.empty())
        {
            auto it = find(s->entityIDs.begin(), s->entityIDs.end(), ID);
            if (it != s->entityIDs.end()) s->entityIDs.erase(it);
        }

        for (Component& c : components)
        {
            switch (c.type)
            {
            default:
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine core error",
                    "Failed to destroy entity '" + to_string(ID)
                    + "' because of unimplemented type!");
            }
            case ComponentType::MESH_3D:
            case ComponentType::MESH_2D:
            {
                Mesh* m{};
                string err = Mesh::GetRegistry().GetContent(c.targetID, m);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Engine core error",
                        "Failed to destroy entity '" + to_string(ID)
                        + "' because it had an invalid mesh! Reason: " + err);
                }
                m->Destroy();

                break;
            };
            case ComponentType::CAMERA_3D:
            case ComponentType::CAMERA_2D:
            {
                //we should not destroy cameras ourselves in elypso engine
                break;
            }
            }
        }

        err = registry.DestroyContent(ID);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso engine entity error",
                "Failed to destroy entity! Reason: " + err);
        }
    }

    Entity::~Entity()
    {
        Log::Print(
            "Destroying entity '" + title + "' with ID '" + to_string(ID) + "'.",
            "EE_ENTITY",
            LogType::INFO);
    }
}