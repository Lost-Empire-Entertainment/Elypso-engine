//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "magic_enum.hpp"

//engine
#include "gui_projectitemslist.hpp"
#include "gameobject.hpp"
#include "core.hpp"
#include "sceneFile.hpp"

using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

using Graphics::Shape::GameObjectManager;
using Core::Engine;
using EngineFile::SceneFile;

namespace Graphics::GUI
{
	void GUIProjectItemsList::RenderProjectItemsList()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoResize;

		string listType = string(magic_enum::enum_name(type));

		if (renderProjectItemsList
			&& ImGui::Begin(listType.c_str(), NULL, windowFlags))
		{
			RenderProjectItemsListContent();

			ImGui::End();
		}
	}

	void GUIProjectItemsList::RenderProjectItemsListContent()
	{
		if (!isContentVectorFilled)
		{
			content.clear();
			selectedScene = "";

			switch (type)
			{
			case Type::Scenes:
				string projectFolder = path(Engine::scenePath).parent_path().parent_path().string();
				for (const auto& entry : directory_iterator(projectFolder))
				{
					cout << entry.path().string() << "\n";

					if (is_directory(entry))
					{
						for (const auto& child : directory_iterator(entry))
						{
							if (is_regular_file(child)
								&& child.path().filename().string() == "scene.txt")
							{
								content.push_back(entry.path().string());
								break;
							}
						}
					}
				}
				break;
			}

			isContentVectorFilled = true;
		}

		ImVec2 childSize = ImVec2(
			ImGui::GetWindowWidth() - 10,
			ImGui::GetWindowHeight() - 100);
		if (ImGui::BeginChild("##content", childSize))
		{
			for (const auto& entry : content)
			{
				string name = path(entry).stem().string();

				if (ImGui::Selectable(name.c_str()))
				{
					for (const auto& child : directory_iterator(entry))
					{
						if (is_regular_file(child)
							&& child.path().filename().string() == "scene.txt")
						{
							selectedScene = child.path().string();
							break;
						}
					}
				}
			}
		}
		ImGui::EndChild();

		ImVec2 buttonSize = ImVec2(100, 30);

		ImVec2 createButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 - buttonSize.x - buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(createButtonPos);
		if (ImGui::Button("Select", buttonSize)
			&& selectedScene != "")
		{
			SceneFile::LoadScene(selectedScene);

			isContentVectorFilled = false;
			renderProjectItemsList = false;
		}

		ImVec2 cancelButtonPos = ImVec2(
			ImGui::GetWindowSize().x / 2 + buttonSize.x / 2,
			ImGui::GetWindowSize().y - 50);
		ImGui::SetCursorPos(cancelButtonPos);
		if (ImGui::Button("Cancel", buttonSize))
		{
			isContentVectorFilled = false;
			renderProjectItemsList = false;
		}
	}
}