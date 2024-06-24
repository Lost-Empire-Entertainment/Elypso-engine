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

//engine
#include "gui_projecthierarchy.hpp"
#include "gui.hpp"
#include "configFile.hpp"
#include "sceneFile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"

using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::cout;
using std::filesystem::exists;
using std::to_string;

using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;
using EngineFile::SceneFile;
using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::String;
using Utils::File;

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
				if (ImGui::MenuItem("Create folder"))
				{
					string cleanedScenePath = String::StringReplace(
						path(Engine::scenePath).parent_path().parent_path().string(), "/", "\\");

					string newFolderPath = cleanedScenePath + "\\New Folder";

					if (exists(newFolderPath))
					{
						//try to create new folder with nr 1 after it
						newFolderPath = cleanedScenePath + "\\New Folder (1)";

						//try to create new folder with first highest available number
						if (exists(newFolderPath))
						{
							int highestNumber = 1;
							for (const auto& entry : directory_iterator(cleanedScenePath))
							{
								path entryPath = entry.path();
								string entryString = entry.path().string();

								if (is_directory(entryPath)
									&& entryString.find("New Folder (") != string::npos)
								{
									vector<string> split = String::Split(entryString, '(');
									string cleanedNumberString = String::StringReplace(split[1], ")", "");
									int number = stoi(cleanedNumberString);

									if (number >= highestNumber) highestNumber = number + 1;

									cout << "found folder with number " << number
										<< ", setting new highest number to " << highestNumber << "\n";
								}
							}

							newFolderPath = cleanedScenePath + "\\New Folder (" + to_string(highestNumber) + ")";
						}
					}

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

					if (ImGui::IsItemHovered()
						&& showPathTooltip)
					{
						ImGui::BeginTooltip();
						ImGui::Text(entry.path.c_str());
						ImGui::EndTooltip();
					}

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Rename"))
						{
							string cleanedEntryPath = String::StringReplace(entry.path, "/", "\\");
							string cleanedScenePath = String::StringReplace(
								path(Engine::scenePath).parent_path().string(), "/", "\\");
							string cleanedModelsFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string() + "\\models", "/", "\\");
							string cleanedTexturesFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string() + "\\textures", "/", "\\");

							if (cleanedEntryPath == cleanedScenePath
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
							}
						}
						if (ImGui::MenuItem("Create folder"))
						{
							string cleanedEntryPath = String::StringReplace(entry.path, "/", "\\");

							string newFolderPath = cleanedEntryPath + "\\New Folder";

							if (exists(newFolderPath))
							{
								//try to create new folder with nr 1 after it
								newFolderPath = cleanedEntryPath + "\\New Folder (1)";

								//try to create new folder with first highest available number
								if (exists(newFolderPath))
								{
									int highestNumber = 1;
									for (const auto& entry : directory_iterator(cleanedEntryPath))
									{
										path entryPath = entry.path();
										string entryString = entry.path().string();

										if (is_directory(entryPath)
											&& entryString.find("New Folder (") != string::npos)
										{
											vector<string> split = String::Split(entryString, '(');
											string cleanedNumberString = String::StringReplace(split[1], ")", "");
											int number = stoi(cleanedNumberString);

											if (number >= highestNumber) highestNumber = number + 1;
										}
									}

									newFolderPath = cleanedEntryPath + "\\New Folder (" + to_string(highestNumber) + ")";
								}
							}

							File::CreateNewFolder(newFolderPath);
						}
						if (ImGui::MenuItem("Delete"))
						{
							string cleanedEntryPath = String::StringReplace(entry.path, "/", "\\");
							string cleanedScenePath = String::StringReplace(
								path(Engine::scenePath).parent_path().string(), "/", "\\");
							string cleanedModelsFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string() + "\\models", "/", "\\");
							string cleanedTexturesFolder = String::StringReplace(
								path(Engine::scenePath).parent_path().string() + "\\textures", "/", "\\");

							if (cleanedEntryPath == cleanedScenePath
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
						if (ImGui::MenuItem("Copy"))
						{
							ConsoleManager::WriteConsoleMessage(
								Caller::ENGINE,
								Type::DEBUG,
								"Copying folder '" + entry.path + "'\n");
						}
						if (ImGui::MenuItem("Paste"))
						{
							ConsoleManager::WriteConsoleMessage(
								Caller::ENGINE,
								Type::DEBUG,
								"Pasting folder '" + entry.path + "'\n");
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
					if (ImGui::Selectable(entry.name.c_str()))
					{
						cout << "clicked on " << entry.path << "\n";
					}

					if (ImGui::IsItemHovered()
						&& showPathTooltip)
					{
						ImGui::BeginTooltip();
						ImGui::Text(entry.path.c_str());
						ImGui::EndTooltip();
					}

					if (ImGui::BeginPopupContextItem())
					{
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
									"Cannot delete file '" + cleanedEntryPath + "' because it is used by this scene!\n");
							}
							else
							{
								ConsoleManager::WriteConsoleMessage(
									Caller::ENGINE,
									Type::DEBUG,
									"Renaming file '" + cleanedEntryPath + "'\n");
							}
						}
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
						if (ImGui::MenuItem("Copy"))
						{
							ConsoleManager::WriteConsoleMessage(
								Caller::ENGINE,
								Type::DEBUG,
								"Copying file '" + entry.path + "'...\n");
						}
						if (ImGui::MenuItem("Paste"))
						{
							ConsoleManager::WriteConsoleMessage(
								Caller::ENGINE,
								Type::DEBUG,
								"Pasting file '" + entry.path + "'...\n");
						}

						ImGui::EndPopup();
					}
				}
			}
		}
		ImGui::EndChild();
	}
}