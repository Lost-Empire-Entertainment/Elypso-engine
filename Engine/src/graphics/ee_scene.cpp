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
    static ElypsoRegistry<Scene> registry{};

    ElypsoRegistry<Scene>& Scene::GetRegistry() { return registry; }

    Scene* Scene::GetActiveScene(u32 windowID)
    {
        const auto& ewreg = EngineWindow::GetRegistry();

        if (!ewreg.createdContent.contains(windowID))
        {
            Log::Print(
                "Couldn't get active scene because engine window '" + to_string(windowID) + "' was not found!",
                "EE_SCENE",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        EngineWindow* ew = ewreg.createdContent[windowID].get();

        if (!registry.createdContent.contains(ew->activeSceneID))
        {
            KalaWindowCore::ForceClose(
                "Active scene check error",
                "Failed to get active scene because engine window '" + to_string(windowID) 
                + "' active scene '" + to_string(ew->activeSceneID) + "' does not exist!");
        }

        return registry.createdContent[ew->activeSceneID].get();
    }

    void Scene::LoadScene(string_view title)
    {
        Scene* targetScene{};

        for (Scene* s : Scene::GetRegistry().runtimeContent)
        {
            if (!s)
            {
                KalaWindowCore::ForceClose(
                    "Scene find error",
                    "Failed to find scene because a nullptr scene was found during scene search!");
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
                "Failed to load scene because scene with title '" + string(title) + "' was not found!",
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
            Log::Print(
                "Failed to create scene '" + string(title) + "' because engine window '" + to_string(windowID) + "' was not found!",
                "EE_SCENE",
                LogType::LOG_ERROR,
                2);

            return nullptr;
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

        //sync to ensure scene gets the highest id from kw
        EngineCore::SyncID();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        scenePtr->ID = newID;
        scenePtr->windowID = windowID;
        scenePtr->title = std::move(title);

        registry.AddContent(newID, std::move(newScene));

        EngineWindow::GetRegistry().createdContent[windowID]->sceneIDs.push_back(newID);

        Log::Print(
			"Created new scene '" + string(title) + "' with ID '" + to_string(newID) + "'!",
			"EE_SCENE",
			LogType::LOG_SUCCESS);

        return scenePtr;
    }

    u32 Scene::GetID() const { return ID; }
    u32 Scene::GetWindowID() const { return windowID; }

    bool Scene::IsActiveScene() const
    {
        for (const auto& ew : EngineWindow::GetRegistry().runtimeContent)
        {
            if (!ew)
            {
                KalaWindowCore::ForceClose(
                    "Active scene check error",
                    "Failed to check if scene '" + to_string(ID) + "' is active because a nullptr engine window was found during scene active check!");
            }

            if (ew->GetID() != windowID) continue;

            if (!ContainsValue(ew->GetSceneIDs(), ID))
            {
                KalaWindowCore::ForceClose(
                    "Active scene check error",
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
            "Set scene '" + title + "' stayAlive state to '" + (stayAlive ? "true" : "false") + "'.",
            "EE_SCENE",
            LogType::LOG_INFO);
    }

    void Scene::SetTitle(string&& newtitle)
    {
        if (newtitle.empty())
        {
            Log::Print(
                "Cannot update scene '" + title + "' title because the new name cannot be empty!",
                "EE_SCENE",
                LogType::LOG_ERROR);

            return;
        }
        if (newtitle.length() > MAX_NAME_LENGTH)
        {
            Log::Print(
                "Cannot update scene '" + title + "' title because the new name cannot be longer than '" + to_string(MAX_NAME_LENGTH) + "' characters!",
                "EE_SCENE",
                LogType::LOG_ERROR);
            
            return;
        }

        for (EngineWindow* ew : EngineWindow::GetRegistry().runtimeContent)
        {
            if (!ew)
            {
                KalaWindowCore::ForceClose(
                    "Scene title change error",
                    "Failed to change scene '" + title + "' title because a nullptr engine window was found during scene existence check!");
            }

            for (u32 s : ew->GetSceneIDs())
            {
                Scene* sc = Scene::GetRegistry().GetContent(s);

                if (!sc)
                {
                    KalaWindowCore::ForceClose(
                        "Scene title change error",
                        "Failed to change scene '" + title + "' title because a nullptr scene was found in engine window '" 
                        + to_string(windowID) + "' during scene existence check!");
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
            "Set scene title to '" + title + "'!",
            "EE_SCENE",
            LogType::LOG_SUCCESS);
    }

    string_view Scene::GetTitle() const { return title; }

    void Scene::LoadScene()
    {
        if (!EngineWindow::GetRegistry().createdContent.contains(windowID))
        {
            KalaWindowCore::ForceClose(
                "Scene load error",
                "Failed to load scene '" + to_string(ID) + "' because engine window '" 
                + to_string(windowID) + "' was not found during scene load!");
        }

        for (Scene* s : registry.runtimeContent)
        {
            if (!s)
            {
                KalaWindowCore::ForceClose(
                    "Scene load error",
                    "Failed to load scene '" + to_string(ID) +  "' because a nullptr scene was found during scene load!");
            }

            if (s->IsActiveScene())
            {
                s->Unload();
                break;
            }
        }

        EngineWindow* ew = EngineWindow::GetRegistry().createdContent[windowID].get();
        ew->activeSceneID = ID;

        //todo: load this scene assets here

        Log::Print(
			"Loaded scene '" + string(title) + "' with ID '" + to_string(ID) + "' from window '" + to_string(windowID) + "'!",
			"EE_SCENE",
			LogType::LOG_SUCCESS);
    }

    void Scene::Unload()
    {
        for (const auto& e : sceneEntities)
        {
            Entity* en = Entity::GetRegistry().GetContent(e);

            if (!en)
            {
                KalaWindowCore::ForceClose(
                    "Scene clear error",
                    "Failed to clear scene '" + title
                    + "' because it had an invalid entity '" + to_string(e) + "'!");
            }

            en->Destroy();
        }

        sceneEntities.clear();

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
        EngineWindow* ew = EngineWindow::GetRegistry().GetContent(windowID);

        if (ew
            && ew->GetSceneIDs().size() > 1
            && title == GetActiveScene(windowID)->title)
        {
            KalaWindowCore::ForceClose(
                "Scene destroy error",
                "The active scene for engine window '" + to_string(windowID) + "' was destroyed!");
        }

        if (ew)
        {
            auto it = find(ew->sceneIDs.begin(), ew->sceneIDs.end(), ID);
            if (it != ew->sceneIDs.end()) ew->sceneIDs.erase(it);
        }

        registry.RemoveContent(ID);
    }

    Scene::~Scene()
    {
        Log::Print(
            "Destroying scene '" + title + "' with ID '" + to_string(ID) + "'.",
            "EE_SCENE",
            LogType::LOG_INFO);

        for (const auto& e : sceneEntities)
        {
            Entity* en = Entity::GetRegistry().GetContent(e);

            if (!en)
            {
                KalaWindowCore::ForceClose(
                    "Scene clear error",
                    "Failed to clear scene '" + title
                    + "' because it had an invalid entity '" + to_string(e) + "'!");
            }

            en->Destroy();
        }

        sceneEntities.clear();

        if (registry.runtimeContent.empty())
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