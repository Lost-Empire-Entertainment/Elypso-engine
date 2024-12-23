//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <memory>
#include <filesystem>
#include <vector>
#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_scenehierarchy.hpp"
#include "gui_engine.hpp"
#include "configFile.hpp"
#include "gameobject.hpp"
#include "core.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "selectobject.hpp"
#include "input.hpp"
#include "meshcomponent.hpp"

using std::shared_ptr;
using std::filesystem::directory_iterator;
using std::filesystem::exists;
using std::filesystem::path;
using std::exception;
using std::vector;
using std::cout;
using std::to_string;

using EngineFile::ConfigFile;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Core::Engine;
using Utils::File;
using Utils::String;
using Core::Select;
using Core::Input;
using Graphics::Components::MeshComponent;

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

			RenderGameobjects();

			ImGui::End();
		}
	}

	void GUISceneHierarchy::RenderGameobjects()
	{
		for (const auto& obj : GameObjectManager::GetObjects())
		{
			if (obj == nullptr) return;

			auto mesh = obj->GetComponent<MeshComponent>();
			if (!mesh
				|| (mesh
				&& mesh->GetMeshType() != MeshComponent::MeshType::actionTex
				&& mesh->GetMeshType() != MeshComponent::MeshType::border
				&& mesh->GetMeshType() != MeshComponent::MeshType::billboard
				&& mesh->GetMeshType() != MeshComponent::MeshType::skybox))
			{
				string name = obj->GetName();
				string label = name + "##" + to_string(obj->GetID());

				bool isSelected = (obj == Select::selectedObj);

				if (isSelected)
				{
					ImVec4 color = ImVec4(1.0f, 1.0f, 0.6f, 1.0f);
					ImGui::PushStyleColor(ImGuiCol_Text, color);
				}

				if (ImGui::Selectable(label.c_str(), isSelected))
				{
					Select::selectedObj = obj;
					Select::isObjectSelected = true;

					if (Input::objectAction == Input::ObjectAction::none)
					{
						Input::objectAction = Input::ObjectAction::move;
					}
					if (Input::axis == "") Input::axis = "X";
				}

				if (isSelected) ImGui::PopStyleColor();

				if (ImGui::BeginPopupContextItem(label.c_str()))
				{
					if (ImGui::MenuItem("Delete"))
					{
						GameObjectManager::DestroyGameObject(obj, false);
					}
					ImGui::EndPopup();
				}
			}
		}
	}
}
#endif