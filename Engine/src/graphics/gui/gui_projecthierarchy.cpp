//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glfw3.h"

//engine
#include "gui_projecthierarchy.hpp"
#include "gui.hpp"
#include "configFile.hpp"
#include "sceneFile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "render.hpp"
#include "gameobject.hpp"

using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::cout;
using std::filesystem::exists;
using std::to_string;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::exception;

using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;
using EngineFile::SceneFile;
using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::String;
using Utils::File;
using Graphics::Render;
using Graphics::Shape::GameObjectManager;

namespace Graphics::GUI
{
	void GUIProjectHierarchy::RenderProjectHierarchy()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		static bool renderProjectHierarchy;
		if (ConfigFileManager::valuesMap.size() > 0)
		{
			renderProjectHierarchy = stoi(ConfigFileManager::valuesMap["gui_projectHierarchy"].GetValue());
		}

		if (renderProjectHierarchy
			&& ImGui::Begin("Project hierarchy", NULL, windowFlags))
		{
			string buttonText = showPathTooltip ? "Hide path" : "Show path";
			if (ImGui::Button(buttonText.c_str()))
			{
				showPathTooltip = !showPathTooltip;
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFileManager::valuesMap["gui_projectHierarchy"].SetValue("0");
			}

			if (ImGui::BeginChild("##content"))
			{
				DisplayDirectoryContents(Engine::sceneParentPath);
			}
			ImGui::EndChild();

			ImGui::End();
		}
	}

	vector<DirectoryEntry> GUIProjectHierarchy::GetDirectoryContents(const string& directoryPath)
	{
		vector<DirectoryEntry> contents;

		for (const auto& entry : directory_iterator(directoryPath))
		{
			DirectoryEntry dirEntry;
			dirEntry.name = entry.path().filename().string();
			dirEntry.path = entry.path().string();

			contents.push_back(dirEntry);
		}

		return contents;
	}

	void GUIProjectHierarchy::DisplayDirectoryContents(const string& directoryPath)
	{
		auto contents = GetDirectoryContents(directoryPath);

		for (const auto& entry : contents)
		{
			if (is_directory(entry.path))
			{
				ImGuiTreeNodeFlags nodeFlags =
					ImGuiTreeNodeFlags_OpenOnArrow
					| ImGuiTreeNodeFlags_OpenOnDoubleClick;
				bool nodeOpen = ImGui::TreeNodeEx(entry.name.c_str(), nodeFlags);

				//hover over folder to show its path in tooltip
				if (ImGui::IsItemHovered()
					&& showPathTooltip)
				{
					ImGui::BeginTooltip();
					ImGui::Text(entry.path.c_str());
					ImGui::EndTooltip();
				}

				if (ImGui::BeginPopupContextItem())
				{
					//create folder inside selected folder
					if (ImGui::MenuItem("Create folder"))
					{
						string cleanedEntryPath = String::CharReplace(entry.path, '/', '\\');

						string newFolderPath = cleanedEntryPath + "\\New Folder";
						newFolderPath = File::AddIndex(cleanedEntryPath, "New Folder");

						File::CreateNewFolder(newFolderPath);
					}
					//delete selected folder
					if (ImGui::MenuItem("Delete"))
					{
						if (nodeOpen)
						{
							string cleanedEntryPath = String::CharReplace(entry.path, '/', '\\');

							ConsoleManager::WriteConsoleMessage(
								Caller::ENGINE,
								Type::EXCEPTION,
								"Cannot delete folder '" + cleanedEntryPath + "' because it is open! Close it before deleting it.\n");
						}
						else Delete(entry.path);
					}

					ImGui::EndPopup();
				}

				if (nodeOpen)
				{
					DisplayDirectoryContents(entry.path);
					ImGui::TreePop();
				}
			}
			else if (is_regular_file(entry.path))
			{
				//if user clicked on file
				if (ImGui::Selectable(entry.name.c_str()))
				{
					cout << "clicked on " << entry.path << "\n";
				}

				//hover over file to show its path in tooltip
				if (ImGui::IsItemHovered()
					&& showPathTooltip)
				{
					ImGui::BeginTooltip();
					ImGui::Text(entry.path.c_str());
					ImGui::EndTooltip();
				}

				if (ImGui::BeginPopupContextItem())
				{
					//open selected scene file
					if (entry.name != "project.txt"
						&& ImGui::MenuItem("Open scene"))
					{
						SceneFile::LoadScene(entry.path);
					}
					//delete selected file
					if (ImGui::MenuItem("Delete"))
					{
						Delete(entry.path);
					}

					ImGui::EndPopup();
				}
			}
		}
	}

	void GUIProjectHierarchy::Delete(const string& targetPath)
	{
		if (is_directory(targetPath))
		{
			string cleanedEntryPath = String::CharReplace(
				targetPath, '/', '\\');
			string cleanedModelsFolder = String::CharReplace(
				Engine::sceneParentPath + "\\models", '/', '\\');
			string cleanedTexturesFolder = String::CharReplace(
				Engine::sceneParentPath + "\\textures", '/', '\\');

			if (cleanedEntryPath == cleanedModelsFolder
				|| cleanedEntryPath == cleanedTexturesFolder)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Cannot delete folder '" + cleanedEntryPath + "' because it is used by this project!\n");
			}
			else
			{
				bool isGameObjectFolder = false;

				vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
				shared_ptr<GameObject> targetObject;
				string name = path(cleanedEntryPath).stem().string();

				//check if an object with the name of the deleted folder exists in the gameobjects list
				for (const shared_ptr<GameObject>& obj : objects)
				{
					if (obj->GetName() == name)
					{
						isGameObjectFolder = true;
						targetObject = obj;
						break;
					}
				}

				if (isGameObjectFolder) GameObjectManager::DestroyGameObject(targetObject);
				else File::DeleteFileOrfolder(cleanedEntryPath);
			}
		}
		else if (is_regular_file(targetPath))
		{
			string cleanedEntryPath = String::CharReplace(targetPath, '/', '\\');
			string cleanedProjectPath = String::CharReplace(Engine::projectPath, '/', '\\');
			string cleanedScenePath = String::CharReplace(Engine::scenePath, '/', '\\');

			if (cleanedEntryPath == cleanedScenePath
				|| cleanedEntryPath == cleanedProjectPath)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Cannot delete file '" + cleanedEntryPath + "' because it is used by this scene!\n");
			}
			else
			{
				string extension = path(cleanedEntryPath).extension().string();
				if (extension == ".fbx"
					|| extension == ".obj"
					|| extension == ".glfw")
				{
					string name = path(cleanedEntryPath).stem().string();
					shared_ptr<GameObject> obj;

					vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
					for (const shared_ptr<GameObject> searchedObj : objects)
					{
						if (searchedObj->GetName() == name)
						{
							obj = searchedObj;
							break;
						}
					}

					if (obj != nullptr)
					{
						GameObjectManager::DestroyGameObject(obj);
					}
					else
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Tried to delete gameobject with name '" + name + "' but it doesn't exist!\n");
					}
				}
				else
				{
					File::DeleteFileOrfolder(cleanedEntryPath);
				}
			}
		}
	}
}