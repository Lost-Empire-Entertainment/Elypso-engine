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
#include "gui_rename.hpp"

using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::cout;
using std::filesystem::exists;
using std::to_string;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

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

namespace Graphics::GUI
{
	void GUIProjectHierarchy::RenderProjectHierarchy()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

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

			DisplayDirectoryContents(path(Engine::scenePath).parent_path().parent_path().string());

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

			dirEntry.isDirectory = entry.is_directory();

			contents.push_back(dirEntry);
		}

		return contents;
	}

	void GUIProjectHierarchy::DisplayDirectoryContents(const string& directoryPath)
	{
		auto contents = GetDirectoryContents(directoryPath);

		if (ImGui::BeginChild("##content"))
		{
			if (ImGui::BeginPopupContextWindow())
			{
				//create new folder inside parent folder
				if (ImGui::MenuItem("Create folder"))
				{
					string cleanedScenePath = String::StringReplace(
						path(Engine::scenePath).parent_path().parent_path().string(), "/", "\\");

					string newFolderPath = cleanedScenePath + "\\New Folder";

					if (exists(newFolderPath)) newFolderPath = File::AddIndex(cleanedScenePath, "New Folder");

					File::CreateNewFolder(newFolderPath);
				}

				ImGui::EndPopup();
			}

			for (const auto& entry : contents)
			{
				if (entry.isDirectory)
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
						//rename selected folder
						if (ImGui::MenuItem("Rename"))
						{
							string cleanedEntryPath = String::StringReplace(entry.path, "/", "\\");
							string cleanedSceneFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string(), "/", "\\");
							string cleanedModelsFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string() + "\\models", "/", "\\");
							string cleanedTexturesFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string() + "\\textures", "/", "\\");

							if (cleanedEntryPath == cleanedSceneFolder
								|| cleanedEntryPath == cleanedModelsFolder
								|| cleanedEntryPath == cleanedTexturesFolder)
							{
								ConsoleManager::WriteConsoleMessage(
									Caller::ENGINE,
									Type::EXCEPTION,
									"Cannot rename folder '" + cleanedEntryPath + "' because it is used by this scene!\n");
							}
							else
							{
								ConsoleManager::WriteConsoleMessage(
									Caller::ENGINE,
									Type::DEBUG,
									"Renaming folder '" + entry.path + "'\n");

								GUIRename::originalName = path(entry.path).stem().string();
								if (is_regular_file(path(entry.path)))
								{
									GUIRename::extension = path(entry.path).extension().string();
								}
								GUIRename::parentFolder = path(entry.path).parent_path().string();

								GUIRename::renderRenameWindow = true;
							}
						}
						//create folder inside selected folder
						if (ImGui::MenuItem("Create folder"))
						{
							string cleanedEntryPath = String::StringReplace(entry.path, "/", "\\");

							string newFolderPath = cleanedEntryPath + "\\New Folder";

							if (exists(newFolderPath)) newFolderPath = File::AddIndex(cleanedEntryPath, "New Folder");

							File::CreateNewFolder(newFolderPath);
						}
						//delete selected folder
						if (ImGui::MenuItem("Delete"))
						{
							string cleanedEntryPath = String::StringReplace(entry.path, "/", "\\");
							string cleanedSceneFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string(), "/", "\\");
							string cleanedModelsFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string() + "\\models", "/", "\\");
							string cleanedTexturesFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string() + "\\textures", "/", "\\");

							if (cleanedEntryPath == cleanedSceneFolder
								|| cleanedEntryPath == cleanedModelsFolder
								|| cleanedEntryPath == cleanedTexturesFolder)
							{
								ConsoleManager::WriteConsoleMessage(
									Caller::ENGINE,
									Type::EXCEPTION,
									"Cannot delete folder '" + cleanedEntryPath + "' because it is used by this scene!\n");
							}
							else
							{
								File::DeleteFileOrfolder(cleanedEntryPath);
							}
						}

						ImGui::EndPopup();
					}

					if (nodeOpen)
					{
						DisplayDirectoryContents(entry.path);
						ImGui::TreePop();
					}
				}
				else
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
						if (entry.name == "scene.txt"
							&& ImGui::MenuItem("Open scene"))
						{
							SceneFile::LoadScene(entry.path);
						}
						//rename selected file
						if (ImGui::MenuItem("Rename"))
						{
							string cleanedEntryPath = String::StringReplace(entry.path, "/", "\\");
							string cleanedProjectPath = String::StringReplace(Engine::projectPath, "/", "\\");
							string cleanedScenePath = String::StringReplace(
								path(Engine::scenePath).parent_path().parent_path().string(), "/", "\\");

							if (cleanedEntryPath == cleanedScenePath
								|| cleanedEntryPath == cleanedProjectPath)
							{
								ConsoleManager::WriteConsoleMessage(
									Caller::ENGINE,
									Type::EXCEPTION,
									"Cannot rename file '" + cleanedEntryPath + "' because it is used by this scene!\n");
							}
							else
							{
								ConsoleManager::WriteConsoleMessage(
									Caller::ENGINE,
									Type::DEBUG,
									"Renaming file '" + cleanedEntryPath + "'\n");

								GUIRename::originalName = path(entry.path).stem().string();
								if (is_regular_file(path(entry.path)))
								{
									GUIRename::extension = path(entry.path).extension().string();
								}
								GUIRename::parentFolder = path(entry.path).parent_path().string();

								GUIRename::renderRenameWindow = true;
							}
						}
						//delete selected file
						if (ImGui::MenuItem("Delete"))
						{
							string cleanedEntryPath = String::StringReplace(entry.path, "/", "\\");
							string cleanedProjectPath = String::StringReplace(Engine::projectPath, "/", "\\");
							string cleanedScenePath = String::StringReplace(Engine::scenePath, "/", "\\");

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
								File::DeleteFileOrfolder(cleanedEntryPath);
							}
						}

						ImGui::EndPopup();
					}
				}
			}
		}
		ImGui::EndChild();
	}
}