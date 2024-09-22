//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
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
#include "sceneFile.hpp"

using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::cout;
using std::filesystem::exists;
using std::to_string;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::exception;

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
			string buttonText = showPathTooltip ? "Hide path" : "Show path";
			if (ImGui::Button(buttonText.c_str()))
			{
				showPathTooltip = !showPathTooltip;
			}

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
		static string chosenEntry = "";

		for (const auto& entry : contents)
		{
			if (chosenEntry == entry.path)
			{
				ImVec4 color = ImVec4(1.0f, 1.0f, 0.6f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, color);
			}

			if (ImGui::Selectable(entry.name.c_str()))
			{
				chosenEntry = entry.path;
				//cout << "clicked on " << entry.path << "\n";
			}

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
					//open selected scene folder
					if (path(entry.path).parent_path().stem().string() == "scenes"
						&& ImGui::MenuItem("Open scene"))
					{
						if (path(entry.path).stem().string() ==
							path(Engine::scenePath).parent_path().stem().string())
						{
							ConsoleManager::WriteConsoleMessage(
								Caller::FILE,
								Type::EXCEPTION,
								"Cannot switch to scene " + path(entry.path).stem().string() + " because it is already open!\n");
						}
						else
						{
							if (SceneFile::unsavedChanges)
							{
								EngineGUI::targetScene = entry.path + "\\scene.txt";
								EngineGUI::renderUnsavedSceneSwitchWindow = true;
							}
							else SceneFile::LoadScene(entry.path + "\\scene.txt");
						}
					}
					//delete selected folder
					if (ImGui::MenuItem("Delete"))
					{
						string scenesFolder = path(Engine::scenePath).parent_path().parent_path().string();

						//if trying to delete scene folder
						if (path(entry.path).parent_path().string() == scenesFolder)
						{
							vector<string> sceneFolderNames;
							for (const auto& sceneFolder : directory_iterator(scenesFolder))
							{
								sceneFolderNames.push_back(path(sceneFolder).string());
							}

							string currentSceneFolder = path(Engine::scenePath).parent_path().string();
							if (entry.path == currentSceneFolder)
							{
								ConsoleManager::WriteConsoleMessage(
									Caller::FILE,
									Type::EXCEPTION,
									"Cannot delete scene folder '" + entry.name + "' because it is the current scene folder!\n");
							}
							else
							{
								if (nodeOpen)
								{
									ConsoleManager::WriteConsoleMessage(
										Caller::FILE,
										Type::EXCEPTION,
										"Cannot delete scene folder '" + entry.name + "' because it is open! Close it before deleting it.\n");
								}
								else File::DeleteFileOrfolder(entry.path);
							}
						}
						//if trying to delete gameobject folder
						else if (path(entry.path).parent_path().string() == Engine::currentGameobjectsPath)
						{
							if (nodeOpen)
							{
								ConsoleManager::WriteConsoleMessage(
									Caller::FILE,
									Type::EXCEPTION,
									"Cannot delete gameobject folder '" + entry.name + "' because it is open! Close it before deleting it.\n");
							}
							else File::DeleteFileOrfolder(entry.path);
						}
						//if trying to delete any other folder
						else
						{
							ConsoleManager::WriteConsoleMessage(
								Caller::FILE,
								Type::EXCEPTION,
								"Cannot delete folder '" + entry.name + "' because it is used by this scene!\n");
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
			else if (is_regular_file(entry.path))
			{
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
					//delete selected file
					if (ImGui::MenuItem("Delete"))
					{
						//if trying to delete texture from textures folder
						if (path(entry.path).parent_path().stem().string() == "textures")
						{
							File::DeleteFileOrfolder(entry.path);
						}
						//if trying to delete any other file
						else
						{
							ConsoleManager::WriteConsoleMessage(
								Caller::FILE,
								Type::EXCEPTION,
								"Cannot delete file '" + entry.name + "' because it is used by this scene!\n");
						}
					}

					ImGui::EndPopup();
				}
			}

			if (chosenEntry == entry.path)
			{
				ImGui::PopStyleColor();
			}
		}
	}
}
#endif