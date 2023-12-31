//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_projecthierarchy.hpp"
#include "console.hpp"

using std::find;
using std::filesystem::is_empty;
using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::directory_iterator;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics::GUI
{
	/// <summary>
	/// These file and folder names are ignored in the rendered project hierarchy.
	/// </summary>
	vector<string> ignoredNames =
	{
		".cmake",
		"CMakeFiles",
		"Testing",
		".ninja_deps",
		".ninja_log",
		"build.ninja",
		"cmake_install.cmake",
		"CMakeCache.txt",
		"CPackConfig.cmake",
		"CPackSourceConfig.cmake",
		"Elypso_engine.exe",
		"EULA.md",
		"glad.lib",
		"LICENSE.md",
		"README.md",
		"VSInheritEnvironments.txt"
	};

	void GUIProjectHierarchy::RenderProjectHierarchy(const path& rootPath)
	{
		ImVec2 initialPos(5, 5);
		ImVec2 initialSize(350, 700);
		ImVec2 maxWindowSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		ImGui::SetNextWindowSizeConstraints(initialSize, maxWindowSize);
		ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);

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

			ImGui::End();
		}
	}
	void GUIProjectHierarchy::DrawFolder(const path& folderPath, bool isRoot)
	{
		ImGuiTreeNodeFlags nodeFlags = 
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (!isRoot)
		{
			bool isFolderOpen = ImGui::TreeNodeEx(
				folderPath.filename().string().c_str(),
				nodeFlags);

			if (isFolderOpen)
			{
				for (const auto& entry : directory_iterator(folderPath))
				{
					if (entry.is_directory()
						&& find(ignoredNames.begin(), ignoredNames.end(), 
							entry.path().filename()) == ignoredNames.end())
					{
						DrawFolder(entry.path());
					}
					else if (entry.is_regular_file()
							 && find(ignoredNames.begin(), ignoredNames.end(),
								 entry.path().filename()) == ignoredNames.end())
					{
						ImGui::BulletText("%s", entry.path().filename().string().c_str());
					}
				}

				ImGui::TreePop();
			}
		}
		else
		{
			for (const auto& entry : directory_iterator(folderPath))
			{
				if (entry.is_directory()
					&& find(ignoredNames.begin(), ignoredNames.end(),
						entry.path().filename()) == ignoredNames.end())
				{
					DrawFolder(entry.path());
				}
				else if (entry.is_regular_file()
						 && find(ignoredNames.begin(), ignoredNames.end(),
							 entry.path().filename()) == ignoredNames.end())
				{
					ImGui::BulletText("%s", entry.path().filename().string().c_str());
				}
			}
		}
	}
}