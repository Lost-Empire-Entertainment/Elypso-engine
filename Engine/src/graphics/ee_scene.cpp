//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>
#include <vector>

#include "core_utils.hpp"
#include "log_utils.hpp"

#include "core/kw_core.hpp"

#include "graphics/ee_scene.hpp"
#include "graphics/ee_window.hpp"
#include "core/ee_core.hpp"
#include "graphics/ee_entity.hpp"

using KalaHeaders::KalaCore::ContainsValue;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::MAX_NAME_LENGTH;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::Scene;

using std::string_view;
using std::to_string;
using std::make_unique;
using std::unique_ptr;
using std::vector;

static vector<FILE*> openFiles{};

namespace ElypsoEngine::Graphics
{
    static EngineRegistry<Scene> registry{};

    EngineRegistry<Scene>& Scene::GetRegistry() { return registry; }

    Scene* Scene::GetActiveScene(u32 windowID)
    {
        EngineWindow* ew{};
        string err = EngineWindow::GetRegistry().GetContent(windowID, ew);
        if (!err.empty())
        {
            Log::Print(
                "Failed to get active scene because engine window was invalid! Reason: " + err,
                "EE_SCENE",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        Scene* scene{};
        err = registry.GetContent(ew->activeSceneID, scene);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso engine scene error",
                "Failed to get active scene because engine window '" 
                + to_string(windowID) + "' active scene was invalid! Reason: " + err);

            return nullptr;
        }

        return scene;
    }

    void Scene::LoadScene(string_view title)
    {
        Scene* targetScene{};

        for (Scene* s : Scene::GetRegistry().GetAllContent())
        {
            if (!s)
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine scene error",
                    "Failed to load scene '" + string(title) 
                    + "' because a nullptr scene was found during scene search!");
            }

            if (s->GetTitle() == title)
            {
                targetScene = s;
                break;
            }
        }

        if (!targetScene)
        {
            Log::Print(
                "Failed to load scene '" + string(title) + "' because it was invalid!",
                "EE_SCENE",
                LogType::LOG_ERROR,
                2);

            return;
        }

        targetScene->LoadScene();
    }

    Scene* Scene::Initialize(
        string&& title,
        u32 windowID,
        path&& escnPath)
    {
        EngineWindow* ew{};
        string err = EngineWindow::GetRegistry().GetContent(windowID, ew);
        if (!err.empty())
        {
            Log::Print(
                "Failed to create scene for engine window '" + to_string(windowID) 
                + "' because the engine window was invalid! Reason: " + err,
                "EE_SCENE",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        if (title.empty()
            || title.length() > MAX_NAME_LENGTH)
        {
            Log::Print(
                "Failed to create scene for engine window '" + to_string(windowID) + "' because its title was empty or too long!",
                "EE_SCENE",
                LogType::LOG_ERROR, 
                2);

            return nullptr;
        }

        for (EngineWindow* ew : EngineWindow::GetRegistry().GetAllContent())
        {
            if (!ew)
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine scene error",
                    "Failed to create scene '" + title 
                    + "' because a nullptr engine window was found during scene existence check!");
            }

            for (u32 s : ew->GetSceneIDs())
            {
                Scene* sc{};
                string err = Scene::GetRegistry().GetContent(s, sc);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Engine scene error",
                        "Failed to create scene because engine window '" 
                        + to_string(windowID) + " had an invalid scene! Reason: " + err);
                }

                if (sc->GetTitle() == title)
                {
                    Log::Print(
                        "Failed to create scene '" + title 
                        + "' because a scene with that title already exists in window '" 
                        + to_string(ew->GetID()) + "'!",
                        "EE_SCENE",
                        LogType::LOG_ERROR,
                        2);

                    return nullptr;
                }
            }
        }

        unique_ptr<Scene> newScene = make_unique<Scene>();
        Scene* scenePtr = newScene.get();

        //sync to ensure scene gets the highest id from kw
        EngineCore::SyncID();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        scenePtr->ID = newID;
        scenePtr->windowID = windowID;
        scenePtr->title = title;

        err = registry.AddContent(newID, std::move(newScene));
        if (!err.empty())
        {
			KalaWindowCore::ForceClose(
				"Elypso engine scene error",
				"Failed to initialize scene! Reason: " + err);
        }

        ew->sceneIDs.push_back(newID);

        Log::Print(
			"Created new scene '" + title 
            + "' with ID '" + to_string(newID) 
            + "' for engine window '" + to_string(windowID) + "'!",
			"EE_SCENE",
			LogType::LOG_SUCCESS);

        return scenePtr;
    }

    u32 Scene::GetID() const { return ID; }
    u32 Scene::GetWindowID() const { return windowID; }

    bool Scene::IsActiveScene() const
    {
        for (EngineWindow* ew : EngineWindow::GetRegistry().GetAllContent())
        {
            if (!ew)
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine scene error",
                    "Failed to check if scene '" + to_string(ID) 
                    + "' is active because a nullptr engine window was found during scene active check!");
            }

            if (ew->GetID() != windowID) continue;

            if (!ContainsValue(ew->GetSceneIDs(), ID))
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine scene error",
                    "Failed to check if scene '" + to_string(ID) 
                    + "' is active in engine window '" + to_string(windowID) 
                    + "' because the engine window does not contain the scene even though it should!");
            }

            if (ew->activeSceneID == ID) return true;
        }

        return false;
    }

    bool Scene::CanStayAlive() const { return stayAlive; }
    void Scene::SetStayAlive(bool newValue)
    { 
        stayAlive = newValue;

        Log::Print(
            "Set scene '" + to_string(ID) + "' stayAlive state to '" + (stayAlive ? "true" : "false") + "'.",
            "EE_SCENE",
            LogType::LOG_INFO);
    }

    void Scene::SetTitle(string&& newtitle)
    {
        if (newtitle.empty()
            || newtitle.length() > MAX_NAME_LENGTH)
        {
            Log::Print(
                "Failed to update scene '" + to_string(ID) + "' title because the new name was empty or too long!",
                "EE_SCENE",
                LogType::LOG_ERROR);

            return;
        }

        for (EngineWindow* ew : EngineWindow::GetRegistry().GetAllContent())
        {
            if (!ew)
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine scene error",
                    "Failed to change scene '" + title 
                    + "' title because a nullptr engine window was found during scene existence check!");
            }

            for (u32 s : ew->GetSceneIDs())
            {
                Scene* sc{};
                string err = Scene::GetRegistry().GetContent(s, sc);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Engine scene error",
                        "Failed to change scene '" + to_string(ID) + "' title because engine window '" 
                        + to_string(windowID) + "' had an invalid scene! Reason: " + err);
                }

                if (sc->GetTitle() == title)
                {
                    Log::Print(
                        "A scene with the title '" + string(newtitle) + "' already exists in window '" + to_string(ew->GetID()) + "'!",
                        "EE_SCENE",
                        LogType::LOG_ERROR);

                    return;
                }
            }
        }

        title = std::move(newtitle);

        Log::Print(
            "Set scene '" + to_string(ID) + "' title to '" + title + "'!",
            "EE_SCENE",  
            LogType::LOG_SUCCESS);
    }

    string_view Scene::GetTitle() const { return title; }

    void Scene::LoadScene()
    {
        EngineWindow* ew{};
        string err = EngineWindow::GetRegistry().GetContent(windowID, ew);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine scene error",
                "Failed to load scene '" + to_string(ID) + "' because engine window '" 
                + to_string(windowID) + "' was invalid! Reason: " + err);
        } 

        for (Scene* s : registry.GetAllContent())
        {
            if (!s)
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine scene error",
                    "Failed to load scene '" + to_string(ID) 
                    +  "' because an invalid scene was found during scene load!");
            }

            if (s->IsActiveScene())
            {
                s->Unload();
                break;
            }
        }

        ew->activeSceneID = ID;

        //TODO: load this scene assets here

        Log::Print(
			"Loaded scene '" + string(title) 
            + "' with ID '" + to_string(ID) 
            + "' for engine window '" + to_string(windowID) + "'!",
			"EE_SCENE",
			LogType::LOG_SUCCESS);
    }

    void Scene::Unload()
    {
        for (const auto& e : sceneIDs)
        {
            Entity* en{};
            string err = Entity::GetRegistry().GetContent(e, en);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine scene error",
                    "Failed to clear scene '" + title 
                    + "' because it had an invalid entity! Reason: " + err);
            }

            en->Destroy();
        }

        sceneIDs.clear();

        for (auto& f : openFiles)
        {
            if (f)
            {
                fclose(f);
                f = nullptr;
            }
        }
        openFiles.clear();
    }

    void Scene::Destroy()
    {
        EngineWindow* ew{};
        string err = EngineWindow::GetRegistry().GetContent(windowID, ew);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso engine scene error",
                "Failed to destroy scene '" + to_string(ID) 
                + "' because its window was invalid! Reason: " + err);
        }

        if (ew->GetSceneIDs().size() > 1
            && title == GetActiveScene(windowID)->title)
        {
            KalaWindowCore::ForceClose(
                "Elypso Engine scene error",
                "The active scene for engine window '" 
                + to_string(windowID) + "' was destroyed!");
        }

        auto it = find(ew->sceneIDs.begin(), ew->sceneIDs.end(), ID);
        if (it != ew->sceneIDs.end()) ew->sceneIDs.erase(it);

        err = registry.DestroyContent(ID);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso engine mesh error",
                "Failed to destroy scene '" + to_string(ID) + "'! Reason: " + err);
        }
    }

    Scene::~Scene()
    {
        Log::Print(
            "Destroying scene '" + to_string(ID) + "'.",
            "EE_SCENE",
            LogType::LOG_INFO);

        for (const auto& e : sceneIDs)
        {
            Entity* en{};
            string err = Entity::GetRegistry().GetContent(e, en);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine scene error",
                    "Failed to destroy scene '" + title
                    + "' because it had an invalid entity! Reason: " + err);
            }

            en->Destroy();
        }

        sceneIDs.clear();

        if (registry.GetAllContent().empty())
        {
            for (auto& f : openFiles)
            {
                if (f)
                {
                    fclose(f);
                    f = nullptr;
                }
            }
            openFiles.clear();
        }
    }
}