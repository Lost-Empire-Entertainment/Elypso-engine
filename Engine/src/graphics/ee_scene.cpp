//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "core/kw_core.hpp"

#include "graphics/ee_scene.hpp"
#include "graphics/ee_entity.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;

using std::to_string;

namespace ElypsoEngine::Graphics
{
    static ElypsoRegistry<Scene> registry{};

    static u32 activeSceneID{};
    static string activeSceneTitle{};

    ElypsoRegistry<Scene>& Scene::GetRegistry() { return registry; }

    void Scene::LoadScene(string_view title)
    {

    }

    u32 Scene::GetActiveSceneID() { return activeSceneID; }
    string Scene::GetActiveSceneTitle()
    { 
        Scene* activeScene = registry.GetContent(activeSceneID);

        if (!activeScene)
        {
            KalaWindowCore::ForceClose(
                "Scene error",
                "Failed to get scene title because active scene was invalid!");
        }

        return activeScene->title;
    }

    Scene* Scene::Initialize(
        string_view title,
        u32 windowID)
    {
        return nullptr;
    }

    u32 Scene::GetID() const { return ID; }
    u32 Scene::GetWindowID() const { return windowID; }

    bool Scene::IsActive() const { return isActive; }

    void Scene::SetTitle(string_view title)
    {

    }

    string_view Scene::GetTitle() const { return title; }

    void Scene::LoadScene()
    {

    }

    void Scene::ClearScene()
    {

    }

    void Scene::Unload()
    {

    }

    void Scene::Destroy()
    {
        for (const auto& e : sceneEntities)
        {
            Entity* en = Entity::GetRegistry().GetContent(e);
            if (en) en->Destroy();
            else
            {
                KalaWindowCore::ForceClose(
                    "Scene destruction error",
                    "Failed to destroy scene '" + to_string(ID)
                    + "' because it had an invalid entity '" + to_string(e) + "'!");
            }
        }

        registry.RemoveContent(ID);
    }

    Scene::~Scene()
    {
        Log::Print(
            "Destroying scene '" + to_string(ID) + "'.",
            "EE_SCENE",
            LogType::LOG_INFO);
    }
}