//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
#include <iostream>
#include <filesystem>

//external
#include "../../../../_external_shared/ImGui/imgui.h"
#include "../../../../_external_shared/ImGui/imgui_impl_glfw.h"
#include "../../../../_external_shared/ImGui/imgui_impl_opengl3.h"
#include "../../../../_external_shared/ImGui/imgui_internal.h"
#include "../../../../_external_shared/GLM/gtc/type_ptr.hpp"
#include "../../../../_external_shared/magic_enum/magic_enum.hpp"

//engine
#include "gui_inspector.hpp"
#include "gui_engine.hpp"
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
#include "gui_projectitemslist.hpp"
#include "fileUtils.hpp"
#include "console.hpp"

using std::cout;
using std::endl;
using std::to_string;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::filesystem::is_regular_file;
using std::filesystem::exists;

using Graphics::Render;
using Core::Select;
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
using EngineFile::ConfigFile;
using Utils::File;
using Core::ConsoleManager;
using ConsoleCaller = Core::ConsoleManager::Caller;
using ConsoleType = Core::ConsoleManager::Type;
using Graphics::Shape::Mesh;

namespace Graphics::GUI
{
	void GUIInspector::RenderInspector()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(2000, 2000));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		bool renderInspector = stoi(ConfigFile::GetValue("gui_inspector"));

		if (renderInspector
			&& ImGui::Begin("Inpsector", NULL, windowFlags))
		{
			if (Select::isObjectSelected
				&& Select::selectedObj->IsInitialized())
			{
				shared_ptr<GameObject> obj = Select::selectedObj;

				bool gameobjectState = obj->IsEnabled();
				if (ImGui::Checkbox("Enable gameobject", &gameobjectState))
				{
					obj->SetEnableState(gameobjectState);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_inspector", "0");
			}

			if (Select::isObjectSelected
				&& Select::selectedObj->IsInitialized())
			{
				Component_GameObject();
				Component_Transform();
				Component_Mesh();
				Component_Material();
			}

			ImGui::End();
		}
	}

	void GUIInspector::Component_GameObject()
	{
		shared_ptr<GameObject>& obj = Select::selectedObj;

		ImGuiChildFlags childWindowFlags{};

		if (ImGui::BeginChild("GameObject", ImVec2(ImGui::GetWindowWidth() - 20, 150), true, childWindowFlags))
		{
			ImGui::Text("GameObject");
			ImGui::Separator();

			string objID = "ID: " + to_string(obj->GetID());
			ImGui::Text(objID.c_str());

			ImGui::SameLine();

			ImGui::SetNextItemWidth(150);

			ImGui::Spacing();

			ImGui::Text("Name");
			string objName = obj->GetName();
#ifdef _WIN32
			strcpy_s(inputTextBuffer_objName, bufferSize, objName.c_str());
#elif __linux__
			strncpy(inputTextBuffer_objName, objName.c_str(), bufferSize);
#endif
			if (ImGui::InputText("##objName", inputTextBuffer_objName, bufferSize))
			{
				if (glfwGetKey(Render::window, GLFW_KEY_ENTER) == GLFW_PRESS)
				{
					bool foundExisting = false;

					string oldName = obj->GetName();

					string newFolderName = inputTextBuffer_objName;
					string newFolderPath = (path(Engine::currentGameobjectsPath) / newFolderName).string();

					for (const auto& folder : directory_iterator(Engine::currentGameobjectsPath))
					{
						foundExisting = (exists(newFolderPath));
						if (foundExisting) break;
					}

					if (foundExisting)
					{
						//resets inputTextBuffer_objName to oldName
						copy(oldName.begin(), oldName.end(), inputTextBuffer_objName);
						inputTextBuffer_objName[oldName.size()] = '\0';

						ConsoleManager::WriteConsoleMessage(
							ConsoleCaller::INPUT,
							ConsoleType::EXCEPTION,
							"Error: Cannot change the name to '" + newFolderName + "' because a gameobject with the same name already exists in this scene " + path(Engine::scenePath).stem().string() + "!");
					}
					else
					{
						string oldFolderPath = (path(Engine::currentGameobjectsPath) / oldName).string();
						File::MoveOrRenameFileOrFolder(oldFolderPath, newFolderPath, true);

						//rename model file if gameobject mesh type is model
						if (obj->GetMesh()->GetMeshType() == Mesh::MeshType::model)
						{
							for (const auto& file : directory_iterator(newFolderPath))
							{
								string oldFileName = path(file).stem().string();
								string oldFilePath = path(file).string();
								string oldFileExtension = path(file).extension().string();
								string extension;

								if (is_regular_file(oldFilePath)
									&& oldFileName == oldName
									&& (path(oldFilePath).extension().string() == ".fbx"
										|| path(oldFilePath).extension().string() == ".obj"
										|| path(oldFilePath).extension().string() == ".glfw"))
								{
									if (path(oldFilePath).extension().string() == ".fbx") extension = ".fbx";
									else if (path(oldFilePath).extension().string() == ".obj") extension = ".obj";
									else if (path(oldFilePath).extension().string() == ".glfw") extension = ".glfw";

									string newFolderNameAndExtension = newFolderName + extension;
									string newFilePath = (path(Engine::currentGameobjectsPath) / newFolderName / newFolderNameAndExtension).string();

									File::MoveOrRenameFileOrFolder(oldFilePath, newFilePath, true);

									break;
								}
							}
						}

						obj->SetName(newFolderName);

						ConsoleManager::WriteConsoleMessage(
							ConsoleCaller::INPUT,
							ConsoleType::INFO,
							"Successfully set gameobject new name to '" + newFolderName + "'!");
							
						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
			}

			ImGui::EndChild();
		}
	}

	void GUIInspector::Component_Transform()
	{
		shared_ptr<GameObject>& obj = Select::selectedObj;

		ImGuiChildFlags childWindowFlags{};

		if (ImGui::BeginChild("Transform", ImVec2(ImGui::GetWindowWidth() - 20, 240), true, childWindowFlags))
		{
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
		}
	}

	void GUIInspector::Component_Mesh()
	{
		shared_ptr<GameObject>& obj = Select::selectedObj;

		int height = obj->GetMesh()->GetMeshType() == Mesh::MeshType::model
			? 75 : 150;

		ImGuiChildFlags childWindowFlags{};

		if (ImGui::BeginChild("Mesh", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags))
		{
			ImGui::Text("Mesh");
			ImGui::Separator();

			Type objType = obj->GetMesh()->GetMeshType();
			string objTypeValue = "Mesh type: " + string(magic_enum::enum_name(objType)) + "   ";
			ImGui::Text(objTypeValue.c_str());

			if (obj->GetMesh()->GetMeshType() != Mesh::MeshType::model)
			{
				bool meshState = obj->GetMesh()->IsEnabled();
				if (ImGui::Checkbox("Enable mesh", &meshState))
				{
					obj->GetMesh()->SetEnableState(meshState);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				bool billboardState = obj->GetChildBillboard()->IsEnabled();
				if (ImGui::Checkbox("Enable billboard", &billboardState))
				{
					obj->GetChildBillboard()->SetEnableState(billboardState);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}

			ImGui::EndChild();
		}
	}

	void GUIInspector::Component_Material()
	{
		shared_ptr<GameObject>& obj = Select::selectedObj;

		ImGuiChildFlags childWindowFlags{};

		float height = Select::selectedObj->GetMesh()->GetMeshType() == Type::spot_light ? 355.0f : 240.0f;
		if (ImGui::BeginChild("Material", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags))
		{
			ImGui::Text("Material");
			ImGui::Separator();

			Mesh::MeshType objType = obj->GetMesh()->GetMeshType();

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
				ImGui::SameLine(ImGui::GetWindowWidth() - 225.0f);
				path diff_texturePath = path(
					Engine::filesPath
					+ obj->GetMaterial()->GetTextureName(Material::TextureType::diffuse));
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button("Diff"))
				{
					GUIProjectItemsList::obj = obj;
					GUIProjectItemsList::textureType = Material::TextureType::diffuse;
					GUIProjectItemsList::type = GUIProjectItemsList::Type::GameobjectTexture;
					GUIProjectItemsList::renderProjectItemsList = true;
				}
				ImGui::PopItemWidth();

				//reset diffuse texture
				ImGui::SameLine(ImGui::GetWindowWidth() - 150.0f);
				path diff_defaultTexturePath = path(path(Engine::filesPath) / "textures" / "diff_default.png");
				string diff_reset = "Reset";
				ImGui::PushID("diffreset");
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button(diff_reset.c_str()))
				{
					string removedTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::diffuse);
					if (removedTexture.find("diff_default.png") == string::npos)
					{
						Texture::LoadTexture(obj, diff_defaultTexturePath.string(), Material::TextureType::diffuse, true);
						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

						if (removedTexture.find("diff_missing.png") == string::npos)
						{
							File::DeleteFileOrfolder(removedTexture);
						}
					}
					else 
					{
						ConsoleManager::WriteConsoleMessage(
							ConsoleCaller::FILE,
							ConsoleType::INFO,
							"Cannot reset texture on diffuse slot for " + obj->GetName() + " because the texture already is default.\n");
					}
				}
				ImGui::PopItemWidth();
				ImGui::PopID();

				ImGui::Spacing();

				//assign specular texture
				ImGui::Text("Specular texture");
				ImGui::SameLine(ImGui::GetWindowWidth() - 225.0f);
				path spec_texturePath = path(
					Engine::filesPath
					+ obj->GetMaterial()->GetTextureName(Material::TextureType::specular));
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button("Spec"))
				{
					GUIProjectItemsList::obj = obj;
					GUIProjectItemsList::textureType = Material::TextureType::specular;
					GUIProjectItemsList::type = GUIProjectItemsList::Type::GameobjectTexture;
					GUIProjectItemsList::renderProjectItemsList = true;
				}
				ImGui::PopItemWidth();

				//reset specular texture
				ImGui::SameLine(ImGui::GetWindowWidth() - 150.0f);
				path spec_defaultTexturePath = path(path(Engine::filesPath) / "textures" / "spec_default.png");
				string spec_reset = "Reset";
				ImGui::PushID("specreset");
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button(spec_reset.c_str()))
				{
					string removedTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::specular);
					if (removedTexture.find("spec_default.png") == string::npos)
					{
						Texture::LoadTexture(obj, spec_defaultTexturePath.string(), Material::TextureType::specular, true);
						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

						File::DeleteFileOrfolder(removedTexture);
					}
					else
					{
						ConsoleManager::WriteConsoleMessage(
							ConsoleCaller::FILE,
							ConsoleType::INFO,
							"Cannot reset texture on specular slot for " + obj->GetName() + " because the texture already is default.\n");
					}
				}
				ImGui::PopItemWidth();
				ImGui::PopID();
			}
			else
			{
				if (objType == Type::point_light)
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
				else if (objType == Type::directional_light)
				{
					vec3 dirDiffuse = obj->GetDirectionalLight()->GetDiffuse();
					ImGui::Text("Directional light diffuse");
					if (ImGui::ColorEdit3("##dirdiff", value_ptr(dirDiffuse)))
					{
						obj->GetDirectionalLight()->SetDiffuse(dirDiffuse);
						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}

					float dirIntensity = obj->GetDirectionalLight()->GetIntensity();
					ImGui::Text("Directional light intensity");
					if (ImGui::DragFloat("##dirint", &dirIntensity, 0.01f, 0.0f, 10.0f))
					{
						obj->GetDirectionalLight()->SetIntensity(dirIntensity);
						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
					ImGui::SameLine();
					if (ImGui::Button("Reset##dirint"))
					{
						obj->GetDirectionalLight()->SetIntensity(1.0f);
						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
			}

			ImGui::EndChild();
		}
	}
}
#endif