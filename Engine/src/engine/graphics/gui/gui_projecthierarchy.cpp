//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glfw3.h"

//engine
#include "gui_projecthierarchy.hpp"
#include "gui_engine.hpp"
#include "configFile.hpp"
#include "sceneFile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "render.hpp"
#include "gameobject.hpp"
#include "sceneFile.hpp"

using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::cout;
using std::filesystem::exists;
using std::to_string;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::exception;
using std::shared_ptr;
using std::ifstream;

using EngineFile::ConfigFile;
using EngineFile::SceneFile;
using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::String;
using Utils::File;
using Graphics::Render;
using Graphics::Shape::GameObjectManager;
using EngineFile::SceneFile;
using Graphics::Shape::GameObject;

namespace Graphics::GUI
{
	void GUIProjectHierarchy::RenderProjectHierarchy()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		bool renderProjectHierarchy = stof(ConfigFile::GetValue("gui_projectHierarchy"));

		if (renderProjectHierarchy
			&& ImGui::Begin("Project hierarchy", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_projectHierarchy", "0");
			}

			if (ImGui::BeginChild("##content"))
			{
				DisplayDirectoryContents(path(Engine::projectPath).parent_path().string());
			}
			ImGui::EndChild();

			ImGui::End();
		}
	}

	void GUIProjectHierarchy::DisplayDirectoryContents(const string& directoryPath)
	{
        static string chosenEntry = "";
        static path toBeDeleted;
        for (const auto& entry : directory_iterator(directoryPath))
        {
            const string fullPath = entry.path().string();
            const string name = entry.path().filename().string();

            bool isSelected = (chosenEntry == fullPath);

            ImGuiTreeNodeFlags nodeFlags =
                ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_OpenOnDoubleClick;
            if (isSelected)
            {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.6f, 1.0f));
            }

            if (entry.is_directory())
            {
                bool nodeOpen = ImGui::TreeNodeEx(name.c_str(), nodeFlags);
                if (ImGui::IsItemClicked())
                {
                    chosenEntry = fullPath;
                }

                if (isSelected) ImGui::PopStyleColor();

                string thisParentFolder = path(entry).parent_path().string();
                if (path(thisParentFolder).stem().string() == "scenes"
                    || path(thisParentFolder).stem().string() == "gameobjects")
                {
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (path(thisParentFolder).stem().string() == "scenes")
                        {
                            string scenesFolder = path(Engine::scenePath).parent_path().parent_path().string();
                            if (thisParentFolder == scenesFolder)
                            {
                                //can open scenes from scene folders
                                if (ImGui::MenuItem("Open scene"))
                                {
                                    if (entry == path(Engine::scenePath).parent_path())
                                    {
                                        ConsoleManager::WriteConsoleMessage(
                                            Caller::FILE,
                                            Type::EXCEPTION,
                                            "Error: Cannot switch to scene '" + path(entry).stem().string() + "' because it is already open!\n");
                                    }
                                    else
                                    {
                                        if (SceneFile::unsavedChanges)
                                        {
                                            EngineGUI::targetScene = (path(entry) / "scene.txt").string();
                                            EngineGUI::renderUnsavedSceneSwitchWindow = true;
                                        }
                                        else SceneFile::LoadScene((path(entry) / "scene.txt").string());
                                    }
                                }
                                else if (ImGui::MenuItem("Delete scene"))
                                {
                                    //can delete scenes from scene folders
                                    if (entry == path(Engine::scenePath).parent_path())
                                    {
                                        ConsoleManager::WriteConsoleMessage(
                                            Caller::FILE,
                                            Type::EXCEPTION,
                                            "Error: Cannot delete scene '" + path(entry).stem().string() + "' because it is currently loaded! Switch to another scene to delete this scene.\n");
                                    }
                                    else
                                    {
                                        toBeDeleted = entry;
                                    }
                                }
                            }
                        }
                        //can delete gameobject parent folders
                        else if (path(thisParentFolder).stem().string() == "gameobjects")
                        {
                            if (ImGui::MenuItem("Delete gameobject"))
                            {
                                for (const auto& child : directory_iterator(entry))
                                {
                                    //look for lights
                                    if (is_regular_file(child)
                                        && path(child).extension().string() == ".txt")
                                    {
                                        string lightTxtFile = path(child).string();
                                        GameObjectManager::FindAndDestroyGameObject(lightTxtFile);

                                        break;
                                    }
                                    else if (is_directory(child))
                                    {
                                        for (const auto& secondChild : directory_iterator(child))
                                        {
                                            if (is_regular_file(secondChild)
                                                && path(secondChild).extension().string() == ".txt")
                                            {
                                                string modelTxtFile = path(secondChild).string();
                                                GameObjectManager::FindAndDestroyGameObject(modelTxtFile);

                                                break;
                                            }
                                        }
                                    }
                                }

                                File::DeleteFileOrfolder(entry);
                            }
                        }
                        ImGui::EndPopup();
                    }
                }

                if (nodeOpen)
                {
                    DisplayDirectoryContents(fullPath);
                    ImGui::TreePop();
                }
            }
            else
            {
                if (ImGui::Selectable(name.c_str(), isSelected))
                {
                    chosenEntry = fullPath;
                }

                if (isSelected) ImGui::PopStyleColor();

                string thisParentFolder = path(entry).parent_path().string();
                string thisExtension = path(entry).extension().string();
                if (path(entry).extension().string() == ".png"
                    || path(entry).extension().string() == ".jpg"
                    || path(entry).extension().string() == ".jpeg")
                {
                    if (ImGui::BeginPopupContextItem())
                    {
                        string gameobjectsFolder = Engine::currentGameobjectsPath;
                        //can delete png, jpg and jpeg files as textures inside gameobject txt file folders
                        if (path(thisParentFolder).stem().string() == "textures")
                        {
                            string texturesFolder = (path(Engine::scenePath).parent_path().parent_path().parent_path() / "textures").string();
                            if (thisParentFolder == texturesFolder
                                && ImGui::MenuItem("Delete texture"))
                            {
                                File::DeleteFileOrfolder(entry);
                            }
                        }
                        //can delete png, jpg and jpeg files as textures inside gameobject txt file folders
                        else if (path(thisParentFolder).parent_path().parent_path().stem().string() == "gameobjects"
                                 && ImGui::MenuItem("Delete texture"))
                        {
                            File::DeleteFileOrfolder(entry);
                        }
                        ImGui::EndPopup();
                    }
                }
            }
        }

        if (toBeDeleted.string() != "")
        {
            File::DeleteFileOrfolder(toBeDeleted);
            toBeDeleted = "";
        }
	}
}
#endif