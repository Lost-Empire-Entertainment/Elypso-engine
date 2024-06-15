//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <filesystem>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "type_ptr.hpp"
#include "magic_enum.hpp"

//engine
#include "gui_inspector.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "gameobject.hpp"
#include "sceneFile.hpp"
#include "input.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "fileexplorer.hpp"
#include "stringUtils.hpp"
#include "configFile.hpp"

using std::cout;
using std::endl;
using std::to_string;
using std::filesystem::path;

using Graphics::Render;
using Physics::Select;
using Graphics::Shape::GameObject;
using Type = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Transform;
using Graphics::Shape::Material;
using EngineFile::SceneFile;
using Core::Input;
using Graphics::Texture;
using Core::Engine;
using EngineFile::FileExplorer;
using Utils::String;
using EngineFile::ConfigFileManager;
using EngineFile::ConfigFileValue;

namespace Graphics::GUI
{
	GUIInspector guiInspector;

	void GUIInspector::RenderInspector()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		static bool renderInspector;
		if (ConfigFileManager::valuesMap.size() > 0)
		{
			renderInspector = stoi(ConfigFileManager::valuesMap["gui_inspector"].GetValue());
		}

		if (renderInspector
			&& ImGui::Begin("Inpsector", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderInspector = false;
			}

			guiInspector.RI_PermanentComponents();

			ImGui::End();
		}
	}
	void GUIInspector::RI_PermanentComponents()
	{
		if (Select::isObjectSelected
			&& Select::selectedObj->IsInitialized())
		{
			shared_ptr<GameObject>& obj = Select::selectedObj;

			ImGuiChildFlags childWindowFlags{};

			ImGui::BeginChild("GameObject", ImVec2(ImGui::GetWindowWidth() - 20, 150), true, childWindowFlags);

			ImGui::Text("GameObject");
			ImGui::Separator();

			string objID = "ID: " + to_string(obj->GetID());
			ImGui::Text(objID.c_str());

			ImGui::SameLine();

			Type objType = obj->GetMesh()->GetMeshType();
			string objTypeValue = "     Mesh type: " + string(magic_enum::enum_name(objType)) + "   ";
			ImGui::Text(objTypeValue.c_str());

			ImGui::SameLine();

			ImGui::SetNextItemWidth(150);

			if (ImGui::BeginCombo("Categories", "", ImGuiComboFlags_NoPreview))
			{
				for (const auto& category : GameObject::categoriesVector)
				{
					if (category != GameObject::Category::cat_All)
					{
						string fullName = string(magic_enum::enum_name(category));
						fullName = String::StringReplace(fullName, "cat_", "");

						size_t pos = fullName.find('_');

						//split the string at the position of the first underscore
						string categoryName = fullName.substr(0, pos);
						string subCategoryName = fullName.substr(pos + 1);
						subCategoryName = String::StringReplace(subCategoryName, "_", " ");
						string displayedCategoryName = categoryName + " - " + subCategoryName;

						shared_ptr<GameObject> obj = Select::selectedObj;
						bool categoryState = obj->GetCategoryState(category);

						ImGui::PushID(fullName.c_str());
						if (ImGui::Checkbox(displayedCategoryName.c_str(), &categoryState))
						{
							obj->SetCategoryState(category, categoryState);
						}
						ImGui::PopID();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Spacing();

			ImGui::Text("Name");
			string objName = obj->GetName();
			strcpy_s(inputTextBuffer_objName, bufferSize, objName.c_str());
			if (ImGui::InputText("##objName", inputTextBuffer_objName, bufferSize))
			{
				obj->SetName(inputTextBuffer_objName);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			ImGui::EndChild();

			ImGui::BeginChild("Transform", ImVec2(ImGui::GetWindowWidth() - 20, 240), true, childWindowFlags);

			ImGui::Text("Transform");
			ImGui::Separator();

			vec3 pos = obj->GetTransform()->GetPosition();
			ImGui::Text("Position");
			if (ImGui::DragFloat3("##objPos", value_ptr(pos), 0.01f))
			{
				obj->GetTransform()->SetPosition(pos);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			vec3 rot = obj->GetTransform()->GetRotation();
			ImGui::Text("Rotation");
			if (ImGui::DragFloat3("##objRot", value_ptr(rot), 0.1f))
			{
				obj->GetTransform()->SetRotation(rot);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			if (rot.x > 359.99f 
				|| rot.x < -359.99f)
			{
				rot.x = 0.0f;
				obj->GetTransform()->SetRotation(rot);
			}
			if (rot.y > 359.99f 
				|| rot.y < -359.99f)
			{
				rot.y = 0.0f;
				obj->GetTransform()->SetRotation(rot);
			}
			if (rot.z > 359.99f 
				|| rot.z < -359.99f)
			{
				rot.z = 0.0f;
				obj->GetTransform()->SetRotation(rot);
			}

			vec3 scale = obj->GetTransform()->GetScale();
			ImGui::Text("Scale");
			if (ImGui::DragFloat3("##objScale", value_ptr(scale), 0.01f))
			{
				obj->GetTransform()->SetScale(scale);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			if (scale.x < 0.0f)
			{
				scale.x = 0.0f;
				obj->GetTransform()->SetScale(scale);
			}
			if (scale.y < 0.0f)
			{
				scale.y = 0.0f;
				obj->GetTransform()->SetScale(scale);
			}
			if (scale.z < 0.0f)
			{
				scale.z = 0.0f;
				obj->GetTransform()->SetScale(scale);
			}

			ImGui::EndChild();

			float height = Select::selectedObj->GetMesh()->GetMeshType() == Type::spot_light ? 355.0f : 240.0f;
			ImGui::BeginChild("Material", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags);

			ImGui::Text("Material");
			ImGui::Separator();

			if (objType == Type::model)
			{
				float modelShininess = obj->GetBasicShape()->GetShininess();
				ImGui::Text("Shininess");

				if (ImGui::DragFloat("##shininess", &modelShininess, 0.1f, 0.5f, 32.0f))
				{
					obj->GetBasicShape()->SetShininess(modelShininess);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##shininess"))
				{
					obj->GetBasicShape()->SetShininess(32.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				ImGui::Spacing();

				//assign diffuse texture
				ImGui::Text("Diffuse texture");
				ImGui::SameLine(ImGui::GetWindowWidth() - 150.0f);
				path diff_texturePath = path(
					Engine::filesPath
					+ obj->GetMaterial()->GetTextureName(Material::TextureType::diffuse));
				string diff_textureName = diff_texturePath.stem().string();
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button(diff_textureName.c_str()))
				{
					string texture = FileExplorer::Select(FileExplorer::SearchType::texture);
					Texture::LoadTexture(obj, texture, Material::TextureType::diffuse, true);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

					cout << "assigned " << texture << " to " << obj->GetName() << "\n";
				}
				ImGui::PopItemWidth();

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text(diff_texturePath.string().c_str());
					ImGui::EndTooltip();
				}

				//reset diffuse texture
				ImGui::SameLine(ImGui::GetWindowWidth() - 225.0f);
				path diff_defaultTexturePath = path(Engine::filesPath + "/textures/diff_default.png");
				string diff_reset = "Reset";
				ImGui::PushID("diffreset");
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button(diff_reset.c_str()))
				{
					Texture::LoadTexture(obj, diff_defaultTexturePath.string(), Material::TextureType::diffuse, true);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

					cout << "removed diffuse from " << obj->GetName() << "\n";
				}
				ImGui::PopItemWidth();
				ImGui::PopID();

				if (ImGui::IsItemHovered())
				{
					string resetsToDefault = "Resets to " + diff_defaultTexturePath.string();
					ImGui::BeginTooltip();
					ImGui::Text(resetsToDefault.c_str());
					ImGui::EndTooltip();
				}

				ImGui::Spacing();

				//assign specular texture
				ImGui::Text("Specular texture");
				ImGui::SameLine(ImGui::GetWindowWidth() - 150.0f);
				path spec_texturePath = path(
					Engine::filesPath
					+ obj->GetMaterial()->GetTextureName(Material::TextureType::specular));
				string spec_textureName = spec_texturePath.stem().string() == "filesEMPTY" ? "None" : spec_texturePath.stem().string();
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button(spec_textureName.c_str()))
				{
					string texture = FileExplorer::Select(FileExplorer::SearchType::texture);
					Texture::LoadTexture(obj, texture, Material::TextureType::specular, true);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

					cout << "assigned " << texture << " to " << obj->GetName() << "\n";
				}
				ImGui::PopItemWidth();

				if (ImGui::IsItemHovered())
				{
					string textureToolTip = spec_texturePath.stem().string() == "filesEMPTY" ? "None" : spec_texturePath.string().c_str();
					ImGui::BeginTooltip();
					ImGui::Text(textureToolTip.c_str());
					ImGui::EndTooltip();
				}

				//remove specular texture
				ImGui::SameLine(ImGui::GetWindowWidth() - 225.0f);
				string spec_reset = "Reset";
				ImGui::PushID("specreset");
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button(spec_reset.c_str()))
				{
					Texture::LoadTexture(obj, "EMPTY", Material::TextureType::specular, false);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

					cout << "removed specular from " << obj->GetName() << "\n";
				}
				ImGui::PopItemWidth();
				ImGui::PopID();

				if (ImGui::IsItemHovered())
				{
					string resetsToDefault = "Removes existing specular texture";
					ImGui::BeginTooltip();
					ImGui::Text(resetsToDefault.c_str());
					ImGui::EndTooltip();
				}
			}
			else if (objType == Type::point_light)
			{
				vec3 pointDiffuse = obj->GetPointLight()->GetDiffuse();
				ImGui::Text("Point light diffuse");
				if (ImGui::ColorEdit3("##pointdiff", value_ptr(pointDiffuse)))
				{
					obj->GetPointLight()->SetDiffuse(pointDiffuse);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float pointIntensity = obj->GetPointLight()->GetIntensity();
				ImGui::Text("Point light intensity");
				if (ImGui::DragFloat("##pointint", &pointIntensity, 0.01f, 0.0f, 10.0f))
				{
					obj->GetPointLight()->SetIntensity(pointIntensity);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##pointint"))
				{
					obj->GetPointLight()->SetIntensity(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float pointDistance = obj->GetPointLight()->GetDistance();
				ImGui::Text("Point light distance");
				if (ImGui::DragFloat("##pointdist", &pointDistance, 0.1f, 0.0f, 100.0f))
				{
					obj->GetPointLight()->SetDistance(pointDistance);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##pointdist"))
				{
					obj->GetPointLight()->SetDistance(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}
			else if (objType == Type::spot_light)
			{
				vec3 spotDiffuse = obj->GetSpotLight()->GetDiffuse();
				ImGui::Text("Spotlight diffuse");
				if (ImGui::ColorEdit3("##spotdiff", value_ptr(spotDiffuse)))
				{
					obj->GetSpotLight()->SetDiffuse(spotDiffuse);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotIntensity = obj->GetSpotLight()->GetIntensity();
				ImGui::Text("Spotlight intensity");
				if (ImGui::DragFloat("##spotint", &spotIntensity, 0.01f, 0.0f, 10.0f))
				{
					obj->GetSpotLight()->SetIntensity(spotIntensity);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotint"))
				{
					obj->GetSpotLight()->SetIntensity(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotDistance = obj->GetSpotLight()->GetDistance();
				ImGui::Text("Spotlight distance");
				if (ImGui::DragFloat("##spotdist", &spotDistance, 0.1f, 0.0f, 100.0f))
				{
					obj->GetSpotLight()->SetDistance(spotDistance);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotdist"))
				{
					obj->GetSpotLight()->SetDistance(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotInnerAngle = obj->GetSpotLight()->GetInnerAngle();
				float spotOuterAngle = obj->GetSpotLight()->GetOuterAngle();

				ImGui::Text("Spotlight inner angle");
				if (ImGui::DragFloat("##spotinnerangle", &spotInnerAngle, 0.1f, 0.0f, spotOuterAngle - 0.01f))
				{
					obj->GetSpotLight()->SetInnerAngle(spotInnerAngle);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotinnerangle"))
				{
					obj->GetSpotLight()->SetInnerAngle(15.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				ImGui::Text("Spotlight outer angle");
				if (ImGui::DragFloat("##spotouterangle", &spotOuterAngle, 0.1f, spotInnerAngle + 0.01f, 180.0f))
				{
					obj->GetSpotLight()->SetOuterAngle(spotOuterAngle);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotouterangle"))
				{
					obj->GetSpotLight()->SetOuterAngle(30.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}

			ImGui::EndChild();
		}
	}
}