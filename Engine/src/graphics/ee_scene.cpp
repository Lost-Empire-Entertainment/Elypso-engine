//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "graphics/ee_window.hpp"
#include "log_utils.hpp"

#include "core/kw_core.hpp"

#include "graphics/ee_scene.hpp"
#include "graphics/ee_entity.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::MAX_NAME_LENGTH;

using std::to_string;
using std::make_unique;
using std::unique_ptr;

namespace ElypsoEngine::Graphics
{
    static bool loadedFirstActiveScene{};

    static ElypsoRegistry<Scene> registry{};

    static u32 activeSceneID = UINT32_MAX;
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
        if (title.empty())
        {
            KalaWindowCore::ForceClose(
                "Scene init error",
                "Failed to create scene because its title was empty!");
        }
        if (title.length() > MAX_NAME_LENGTH)
        {
            KalaWindowCore::ForceClose(
                "Scene init error",
                "Failed to create scene because its title was too long!");
        }

        if (!EngineWindow::GetRegistry().createdContent.contains(windowID))
        {
            KalaWindowCore::ForceClose(
                "Scene init error",
                "Failed to create scene because engine window '" + to_string(windowID) + "' was not found!");
        }

        for (EngineWindow* ew : EngineWindow::GetRegistry().runtimeContent)
        {
            if (!ew)
            {
                KalaWindowCore::ForceClose(
                    "Scene init error",
                    "Failed to create scene because a nullptr engine window was found during scene existence check!");
            }

            for (u32 s : ew->GetSceneIDs())
            {
                Scene* sc = Scene::GetRegistry().GetContent(s);

                if (!sc)
                {
                    KalaWindowCore::ForceClose(
                        "Scene init error",
                        "Failed to create scene because a nullptr scene was found in engine window '" 
                        + to_string(windowID) + "' during scene existence check!");
                }

                if (sc->GetTitle() == title)
                {
                    KalaWindowCore::ForceClose(
                        "Scene init error",
                        "A scene with the title '" + string(title) + "' already exists in window '" + to_string(ew->GetID()) + "'!");
                }
            }
        }

        unique_ptr<Scene> newScene = make_unique<Scene>();
        Scene* scenePtr = newScene.get();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        scenePtr->ID = newID;
        scenePtr->windowID = windowID;
        scenePtr->title = title;

        registry.AddContent(newID, std::move(newScene));

        Log::Print(
			"Created new scene '" + string(title) + "' with ID '" + to_string(newID) + "'!",
			"EE_SCENE",
			LogType::LOG_SUCCESS);

        return scenePtr;
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
        Scene* activeScene = registry.GetContent(activeSceneID);
        if (loadedFirstActiveScene
            && !activeScene)
        {
            KalaWindowCore::ForceClose(
                "Scene load error",
                "Failed to get active scene to unload it because it was not found!");
        }

        if (activeScene == this)
        {
            Log::Print(
                "Cannot load scene '" + to_string(activeScene->ID) + "' because it is already loaded!",
                "EE_SCENE",
                LogType::LOG_ERROR,
                2);

            return;
        }

        if (activeScene) activeScene->Unload();

        //todo: load this scene assets here

        activeSceneID = ID;
        activeSceneTitle = title;

        loadedFirstActiveScene = true;

        Log::Print(
			"Loaded scene '" + string(title) + "' with ID '" + to_string(ID) + "' from window '" + to_string(windowID) + "'!",
			"EE_SCENE",
			LogType::LOG_SUCCESS);
    }

    void Scene::ClearScene()
    {

    }

    void Scene::Unload()
    {
        //todo: unload loaded scene assets here
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