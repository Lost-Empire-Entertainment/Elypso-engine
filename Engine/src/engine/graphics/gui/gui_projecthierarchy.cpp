//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#include <fstream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_projecthierarchy.hpp"
#include "gui.hpp"
#include "console.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "sceneFile.hpp"
#include "render.hpp"

using std::cout;
using std::endl;
using std::find;
using std::ifstream;
using std::filesystem::is_empty;
using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::directory_iterator;

using Utils::String;
using Utils::File;
using Graphics::Render;
using EngineFile::SceneFile;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::GUI
{
	void GUIProjectHierarchy::RenderProjectHierarchy(const path& rootPath)
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (renderProjectHierarchy
			&& ImGui::Begin("Project hierarchy", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderProjectHierarchy = false;
			}

			DrawFolder(rootPath, true);
			RightClickPopup();
			RenameFileOrFolder();

			ImGui::End();
		}
	}
	void GUIProjectHierarchy::DrawFolder(const path& folderPath, bool isRoot)
	{
		ImGuiTreeNodeFlags nodeFlags = 
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick;

		//always keep the root folder open
		if (isRoot) nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

		bool isFolderOpen = ImGui::TreeNodeEx(
			folderPath.filename().string().c_str(),
			nodeFlags);

		if (ImGui::IsItemHovered()
			&& ImGui::IsMouseClicked(1))
		{
			selectedItemPath = folderPath;
			rightMouseClicked = true;
			isFolder = true;
		}

		if (isFolderOpen)
		{
			for (const auto& entry : directory_iterator(folderPath))
			{
				if (entry.is_directory()) DrawFolder(entry.path(), false);
				else if (entry.is_regular_file())
				{
					ImGui::Selectable(entry.path().filename().string().c_str());

					if (ImGui::IsItemHovered()
						&& ImGui::IsMouseClicked(1))
					{
						selectedItemPath = entry.path();
						rightMouseClicked = true;
						isFolder = false;
					}
				}
			}

			ImGui::TreePop();
		}
	}
	void GUIProjectHierarchy::RightClickPopup()
	{
		if (rightMouseClicked)
		{
			string uncleanSelectedItemPath = selectedItemPath.string();
			uncleanSelectedItemPath = String::CharReplace(uncleanSelectedItemPath, '\\', '/');
			uncleanSelectedItemPath = String::CharReplace(uncleanSelectedItemPath, '"', '\0');
			selectedItemPath = uncleanSelectedItemPath;
			string displayedPath = uncleanSelectedItemPath;

			ImGui::OpenPopup("rightclickpopup");

			rightMouseClicked = false;
		}

		if (ImGui::BeginPopupContextItem("rightclickpopup"))
		{
			if (ImGui::MenuItem("Open"))
			{
				if (selectedItemPath.extension() != ".txt")
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Target file is not an openable file! Only txt files are supported.\n\n");
					return;
				}

				if (selectedItemPath.stem() == "project"
					&& selectedItemPath.extension() == ".txt")
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Target file is a project file and cannot be run! Please choose a valid scene file.\n\n");
					return;
				}

				if (SceneFile::unsavedChanges)
				{
					EngineGUI::targetScene = selectedItemPath.string();
					EngineGUI::renderUnsavedSceneSwitchWindow = true;
				}
				else SceneFile::LoadScene(selectedItemPath.string());
			}

			if (ImGui::MenuItem("Create folder")
				&& isFolder)
			{
				path output = selectedItemPath / "New folder";
				File::CreateNewFolder(output);

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Rename"))
			{
				if (selectedItemPath == rootPath)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Cannot rename root path!\n\n");
					return;
				}

				isRenaming = true;
			}

			if (ImGui::MenuItem("Copy"))
			{
				if (selectedItemPath == rootPath)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Cannot copy root path!\n\n");
					return;
				}

				copyPath = selectedItemPath;
				cout << "Copied " << selectedItemPath.string() << endl;
			}

			if (ImGui::MenuItem("Paste"))
			{
				if (!isFolder)
				{
					if (selectedItemPath == rootPath)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Path is not a folder!\n\n");
						return;
					}
				}

				if (copyPath.empty()
					|| !exists(copyPath))
				{
					if (selectedItemPath == rootPath)
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"There is nothing to paste!\n\n");
						return;
					}
				}

				cout << "Pasted " << copyPath.string() << " to " << selectedItemPath.string() << endl;
				File::CopyFileOrFolder(copyPath, selectedItemPath);

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			if (ImGui::MenuItem("Delete"))
			{
				if (selectedItemPath == rootPath)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Cannot delete root path!\n\n");
					return;
				}

				path currentScenePath = SceneFile::currentScenePath;
				if (selectedItemPath == currentScenePath)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Cannot delete opened scene file!\n\n");
					return;
				}
				path currentSceneParentPath = currentScenePath.parent_path();
				if (selectedItemPath == currentSceneParentPath)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Cannot delete opened scene folder!\n\n");
					return;
				}

				cout << "Deleted " << selectedItemPath.string() << endl;
				File::DeleteFileOrfolder(selectedItemPath);
				selectedItemPath = path();

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::EndPopup();
		}
	}

	void GUIProjectHierarchy::RenameFileOrFolder()
	{
		if (isRenaming)
		{
			ImGui::OpenPopup("Rename item");
			isRenaming = false;
		}

		if (ImGui::BeginPopupModal("Rename item", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Enter new name:");
			ImGui::InputText("##newname", newNameBuffer, sizeof(newNameBuffer));

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				//check if the new name is not empty
				if (strlen(newNameBuffer) > 0)
				{
					path newPath = selectedItemPath.parent_path() / newNameBuffer;

					//check if the new path is different from the current path
					if (newPath != selectedItemPath)
					{
						File::MoveOrRenameFileOrFolder(selectedItemPath, newPath, true);
					}
				}

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}