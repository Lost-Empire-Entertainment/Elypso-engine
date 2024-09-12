//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>
#include <filesystem>
#include <vector>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_scenehierarchy.hpp"
#include "gui.hpp"
#include "configFile.hpp"
#include "gameobject.hpp"
#include "core.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "selectobject.hpp"

using std::shared_ptr;
using std::filesystem::directory_iterator;
using std::filesystem::exists;
using std::filesystem::path;
using std::exception;
using std::vector;

using EngineFile::ConfigFile;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Core::Engine;
using Utils::File;
using Utils::String;
using Physics::Select;

namespace Graphics::GUI
{
	void GUISceneHierarchy::RenderSceneHierarchy()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		bool renderSceneHierarchy = stoi(ConfigFile::GetValue("gui_sceneHierarchy"));

		if (renderSceneHierarchy
			&& ImGui::Begin("Scene hierarchy", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_sceneHierarchy", "0");
			}

			DisplayGameObjects();

			ImGui::End();
		}
	}

	void GUISceneHierarchy::DisplayGameObjects()
	{
		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();

		for (const shared_ptr<GameObject>& obj : objects)
		{
			//ignores empty gameobjects
			if (obj == nullptr) continue;
			//ignores billboards for lightsources
			if (obj->GetParentBillboardHolder() != nullptr) continue;

			string name = obj->GetName();

			if (obj == Select::selectedObj)
			{
				ImVec4 color = ImVec4(1.0f, 1.0f, 0.6f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, color);
			}

			if (ImGui::Selectable(name.c_str()))
			{
				Select::selectedObj = obj;
				Select::isObjectSelected = true;
			}

			if (obj == Select::selectedObj)
			{
				ImGui::PopStyleColor();
			}

			if (ImGui::BeginPopupContextItem())
			{
				//delete selected gameobject
				if (ImGui::MenuItem("Delete"))
				{
					GameObjectManager::DestroyGameObject(obj);
				}

				ImGui::EndPopup();
			}
		}
	}
}