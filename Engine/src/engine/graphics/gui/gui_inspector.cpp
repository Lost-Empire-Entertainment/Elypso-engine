//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.
#if ENGINE_MODE
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
#include "transformcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lightcomponent.hpp"
#include "directionallight.hpp"
#include "spotlight.hpp"
#include "pointlight.hpp"
#include "billboard.hpp"

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
using Graphics::Shape::GameObjectManager;
using Graphics::Components::TransformComponent;
using Graphics::Components::MeshComponent;
using Graphics::Components::MaterialComponent;
using Graphics::Components::LightComponent;
using MeshType = Graphics::Components::MeshComponent::MeshType;
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
using Graphics::Shape::DirectionalLight;
using Graphics::Shape::SpotLight;
using Graphics::Shape::PointLight;
using Graphics::Shape::Billboard;

namespace Graphics::GUI
{
	void GUIInspector::RenderInspector()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_AlwaysVerticalScrollbar;;

		bool renderInspector = stoi(ConfigFile::GetValue("gui_inspector"));

		if (renderInspector
			&& ImGui::Begin("Inpsector", NULL, windowFlags))
		{
			if (Select::isObjectSelected) AddComponent();

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_inspector", "0");
			}

			if (Select::isObjectSelected)
			{
				Component_GameObject();
				Component_Transform();
				Component_Mesh();
				Component_Material();
				Component_Light();
			}

			ImGui::End();
		}
	}

	void GUIInspector::AddComponent()
	{
		const char* items[] = { "Mesh", "Material", "Light" };
		static int currentItem = -1;

		const char* currentLabel = (currentItem >= 0) ? items[currentItem] : "Add component";

		if (ImGui::BeginCombo("##AddComponentDropdown", currentLabel))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				if (ImGui::Selectable(items[i], currentItem == i))
				{
					currentItem = i;
					cout << "Selected " << items[i] << "\n";

					if (strcmp(items[i], "Mesh") == 0)
					{
						auto existingMesh = Select::selectedObj->GetComponent<MeshComponent>();
						if (existingMesh)
						{
							ConsoleManager::WriteConsoleMessage(
								ConsoleCaller::INPUT,
								ConsoleType::EXCEPTION,
								"Error: " + Select::selectedObj->GetName() + " already has a mesh component!");
						}
						else
						{

						}
					}
					else if (strcmp(items[i], "Material") == 0)
					{
						auto existingMat = Select::selectedObj->GetComponent<MaterialComponent>();
						if (existingMat)
						{
							ConsoleManager::WriteConsoleMessage(
								ConsoleCaller::INPUT,
								ConsoleType::EXCEPTION,
								"Error: " + Select::selectedObj->GetName() + " already has a material component!");
						}
						else
						{

						}
					}
					else if (strcmp(items[i], "Light") == 0)
					{
						auto existingLight = Select::selectedObj->GetComponent<LightComponent>();
						if (existingLight)
						{
							ConsoleManager::WriteConsoleMessage(
								ConsoleCaller::INPUT,
								ConsoleType::EXCEPTION,
								"Error: " + Select::selectedObj->GetName() + " already has a light component!");
						}
						else
						{
							auto pointLight = Select::selectedObj->AddComponent<LightComponent>(
								vec3(1),
								1.0f,
								1.0f);
						}
					}

					currentItem = -1;
					break;
				}
			}
			ImGui::EndCombo();
		}
	}

	void GUIInspector::Component_GameObject()
	{
		shared_ptr<GameObject>& obj = Select::selectedObj;

		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("GameObject", ImVec2(ImGui::GetWindowWidth() - 20, 180), true, childWindowFlags);
		
		ImGui::Text("GameObject");
		ImGui::Separator();

		string objID = "ID: " + to_string(obj->GetID());
		ImGui::Text("%s", objID.c_str());

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
					auto mesh = obj->GetComponent<MeshComponent>();
					if (mesh
						&& mesh->GetMeshType() == MeshComponent::MeshType::model)
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

		if (Select::isObjectSelected)
		{
			shared_ptr<GameObject> obj = Select::selectedObj;

			bool gameobjectState = obj->IsEnabled();
			if (ImGui::Checkbox("Enable gameobject", &gameobjectState))
			{
				obj->SetEnableState(gameobjectState);

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
		}

		ImGui::EndChild();
	}

	void GUIInspector::Component_Transform()
	{
		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("Transform", ImVec2(ImGui::GetWindowWidth() - 20, 240), true, childWindowFlags);
		
		ImGui::Text("Transform");
		ImGui::Separator();

		auto& obj = Select::selectedObj;

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

	void GUIInspector::Component_Mesh()
	{
		auto& obj = Select::selectedObj;

		auto mesh = obj->GetComponent<MeshComponent>();

		bool meshExists = mesh != nullptr;
		float height = meshExists ? 150 : 100;

		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("Mesh", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags);
		
		ImGui::Text("Mesh");

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
		if (ImGui::Button("X"))
		{
			Select::selectedObj->RemoveComponent<MeshComponent>();
		}

		ImGui::Separator();

		if (!mesh)
		{
			ImGui::Text("Mesh is missing.");
		}
		else
		{
			MeshType objType = mesh->GetMeshType();
			string objTypeValue = "Mesh type: " + string(magic_enum::enum_name(objType)) + "   ";
			ImGui::Text("%s", objTypeValue.c_str());

			if (mesh->GetMeshType() != MeshComponent::MeshType::model)
			{
				bool meshState = mesh->IsEnabled();
				if (ImGui::Checkbox("Enable mesh", &meshState))
				{
					mesh->SetEnableState(meshState);

					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				shared_ptr<GameObject> childBillboard = nullptr;
				if (obj->GetChildren().size() > 0)
				{
					for (const auto& child : obj->GetChildren())
					{
						if (mesh->GetMeshType() == MeshComponent::MeshType::billboard)
						{
							childBillboard = obj;
							break;
						}
					}
				}

				if (childBillboard != nullptr)
				{
					bool billboardState = childBillboard->IsEnabled();
					if (ImGui::Checkbox("Enable billboard", &billboardState))
					{
						childBillboard->SetEnableState(billboardState);

						if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
					}
				}
			}
		}

		ImGui::EndChild();
	}

	void GUIInspector::Component_Material()
	{
		auto& obj = Select::selectedObj;

		auto mesh = obj->GetComponent<MeshComponent>();
		auto mat = obj->GetComponent<MaterialComponent>();

		float height;
		if (mesh
			&& mat)
		{
			height = 270;
		}
		else height = 100;

		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("Material", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags);
		
		ImGui::Text("Material");

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
		if (ImGui::Button("X"))
		{
			Select::selectedObj->RemoveComponent<MaterialComponent>();
		}

		ImGui::Separator();

		if (!mesh)
		{
			ImGui::Text("Mesh is missing.");
		}
		else if (!mat)
		{
			ImGui::Text("Material is missing.");
		}
		else if (!mesh
			&& !mat)
		{
			ImGui::Text("Mesh and material are missing.");
		}
		else
		{
			MeshComponent::MeshType objType = mesh->GetMeshType();

			if (objType == MeshType::model)
			{
				const string& vertShader = mat->GetShaderName(0);
				const string& fragShader = mat->GetShaderName(1);
				const string& diffTexture = mat->GetTextureName(MaterialComponent::TextureType::diffuse);
				const string& specTexture = mat->GetTextureName(MaterialComponent::TextureType::specular);

				ImGui::Button("Vertex shader");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text(vertShader.c_str());
					ImGui::EndTooltip();
				}
				ImGui::Button("Fragment shader");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text(fragShader.c_str());
					ImGui::EndTooltip();
				}

				ImGui::Button("Diffuse texture");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text(diffTexture.c_str());
					ImGui::EndTooltip();
				}
				ImGui::Button("Specular texture");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text(specTexture.c_str());
					ImGui::EndTooltip();
				}

				/*
				*
				* SHININESS IS CURRENTLY DISABLED BECAUSE IT IS UNUSED
				* IT WILL BE RE-ENABLED IN A FUTURE UPDATE
				*
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
				*/

				//assign diffuse texture
				ImGui::Text("Diffuse texture");
				ImGui::SameLine(ImGui::GetWindowWidth() - 225.0f);
				path diff_texturePath = path(
					Engine::filesPath
					+ mat->GetTextureName(MaterialComponent::TextureType::diffuse));
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button("Diff"))
				{
					GUIProjectItemsList::obj = obj;
					GUIProjectItemsList::textureType = MaterialComponent::TextureType::diffuse;
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
					string removedTexture = mat->GetTextureName(MaterialComponent::TextureType::diffuse);
					if (removedTexture.find("diff_default.png") == string::npos)
					{
						Texture::LoadTexture(obj, diff_defaultTexturePath.string(), MaterialComponent::TextureType::diffuse, true);
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
					+ mat->GetTextureName(MaterialComponent::TextureType::specular));
				ImGui::PushItemWidth(200.0f);
				if (ImGui::Button("Spec"))
				{
					GUIProjectItemsList::obj = obj;
					GUIProjectItemsList::textureType = MaterialComponent::TextureType::specular;
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
					string removedTexture = mat->GetTextureName(MaterialComponent::TextureType::specular);
					if (removedTexture.find("spec_default.png") == string::npos)
					{
						Texture::LoadTexture(obj, spec_defaultTexturePath.string(), MaterialComponent::TextureType::specular, true);
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
				auto& childBillboard = obj->GetChildBillboard();
				auto childMat = childBillboard->GetComponent<MaterialComponent>();
				if (childMat)
				{
					const string& vertShader = mat->GetShaderName(0);
					const string& fragShader = mat->GetShaderName(1);

					const string& childVertShader = childMat->GetShaderName(0);
					const string& childFragShader = childMat->GetShaderName(1);

					const string& diffTexture = childMat->GetTextureName(MaterialComponent::TextureType::diffuse);

					ImGui::Button("Vertex shader");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text(vertShader.c_str());
						ImGui::EndTooltip();
					}
					ImGui::Button("Fragment shader");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text(fragShader.c_str());
						ImGui::EndTooltip();
					}

					ImGui::Button("Billboard vertex shader");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text(childVertShader.c_str());
						ImGui::EndTooltip();
					}
					ImGui::Button("Billboard fragment shader");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text(childFragShader.c_str());
						ImGui::EndTooltip();
					}

					ImGui::Button("Billboard diffuse texture");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text(diffTexture.c_str());
						ImGui::EndTooltip();
					}
				}
			}
		}

		ImGui::EndChild();
	}

	void GUIInspector::Component_Light()
	{
		auto& obj = Select::selectedObj;

		auto mesh = obj->GetComponent<MeshComponent>();
		auto light = obj->GetComponent<LightComponent>();

		bool bothExist =
			mesh != nullptr
			&& light != nullptr;

		float height;
		if (!bothExist) height = 100;
		else height = mesh->GetMeshType() == MeshType::spot_light ? 400 : 270;

		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("Light", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags);
		
		ImGui::Text("Light");

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
		if (ImGui::Button("X"))
		{
			obj->RemoveComponent<LightComponent>();
		}

		ImGui::Separator();

		if (!mesh)
		{
			ImGui::Text("Mesh is missing.");
		}
		else if (!light)
		{
			ImGui::Text("Light is missing.");
		}
		else if (!mesh
			&& !light)
		{
			ImGui::Text("Light and mesh are missing.");
		}
		else
		{
			ChangeLightType();

			MeshComponent::MeshType objType = mesh->GetMeshType();
			if (objType == MeshType::point_light)
			{
				vec3 pointDiffuse = light->GetDiffuse();
				ImGui::Text("Point light diffuse");
				if (ImGui::ColorEdit3("##pointdiff", value_ptr(pointDiffuse)))
				{
					light->SetDiffuse(pointDiffuse);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float pointIntensity = light->GetIntensity();
				ImGui::Text("Point light intensity");
				if (ImGui::DragFloat("##pointint", &pointIntensity, 0.01f, 0.0f, 10.0f))
				{
					light->SetIntensity(pointIntensity);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##pointint"))
				{
					light->SetIntensity(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float pointDistance = light->GetDistance();
				ImGui::Text("Point light distance");
				if (ImGui::DragFloat("##pointdist", &pointDistance, 0.1f, 0.0f, 100.0f))
				{
					light->SetDistance(pointDistance);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##pointdist"))
				{
					light->SetDistance(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}
			else if (objType == MeshType::spot_light)
			{
				vec3 spotDiffuse = light->GetDiffuse();
				ImGui::Text("Spotlight diffuse");
				if (ImGui::ColorEdit3("##spotdiff", value_ptr(spotDiffuse)))
				{
					light->SetDiffuse(spotDiffuse);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotIntensity = light->GetIntensity();
				ImGui::Text("Spotlight intensity");
				if (ImGui::DragFloat("##spotint", &spotIntensity, 0.01f, 0.0f, 10.0f))
				{
					light->SetIntensity(spotIntensity);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotint"))
				{
					light->SetIntensity(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotDistance = light->GetDistance();
				ImGui::Text("Spotlight distance");
				if (ImGui::DragFloat("##spotdist", &spotDistance, 0.1f, 0.0f, 100.0f))
				{
					light->SetDistance(spotDistance);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotdist"))
				{
					light->SetDistance(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float spotInnerAngle = light->GetInnerAngle();
				float spotOuterAngle = light->GetOuterAngle();

				ImGui::Text("Spotlight inner angle");
				if (ImGui::DragFloat("##spotinnerangle", &spotInnerAngle, 0.1f, 0.0f, spotOuterAngle - 0.01f))
				{
					light->SetInnerAngle(spotInnerAngle);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotinnerangle"))
				{
					light->SetInnerAngle(15.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				ImGui::Text("Spotlight outer angle");
				if (ImGui::DragFloat("##spotouterangle", &spotOuterAngle, 0.1f, spotInnerAngle + 0.01f, 180.0f))
				{
					light->SetOuterAngle(spotOuterAngle);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##spotouterangle"))
				{
					light->SetOuterAngle(30.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}
			else if (objType == MeshType::directional_light)
			{
				vec3 dirDiffuse = light->GetDiffuse();
				ImGui::Text("Directional light diffuse");
				if (ImGui::ColorEdit3("##dirdiff", value_ptr(dirDiffuse)))
				{
					light->SetDiffuse(dirDiffuse);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}

				float dirIntensity = light->GetIntensity();
				ImGui::Text("Directional light intensity");
				if (ImGui::DragFloat("##dirint", &dirIntensity, 0.01f, 0.0f, 10.0f))
				{
					light->SetIntensity(dirIntensity);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset##dirint"))
				{
					light->SetIntensity(1.0f);
					if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
				}
			}
		}

		ImGui::EndChild();
	}

	void GUIInspector::ChangeLightType()
	{
		string lightTypes[] = { "Point light", "Spotlight", "Directional light" };
		static int currentItem = -1;
		if (ImGui::BeginCombo("##ChangeLightTypeDropdown", "Change light type"))
		{
			for (int i = 0; i < IM_ARRAYSIZE(lightTypes); i++)
			{
				if (ImGui::Selectable(lightTypes[i].c_str(), currentItem == i))
				{
					currentItem = i;

					string value = lightTypes[i];
					
					MeshType type = Select::selectedObj->GetComponent<MeshComponent>()->GetMeshType();
					vec3 oldPos = Select::selectedObj->GetTransform()->GetPosition();
					vec3 oldRot = Select::selectedObj->GetTransform()->GetRotation();
					vec3 oldScale = Select::selectedObj->GetTransform()->GetScale();

					auto obj = Select::selectedObj;
					string oldTxtPath = obj->GetTxtFilePath();
					auto oldBillboard = obj->GetChildBillboard();
					vec3 oldBillboardPos = obj->GetTransform()->GetPosition();
					unsigned int oldBillboardID = Select::selectedObj->GetChildBillboard()->GetID();

					if (value == "Point light")
					{
						if (type == MeshType::point_light)
						{
							ConsoleManager::WriteConsoleMessage(
								ConsoleCaller::INPUT,
								ConsoleType::EXCEPTION,
								"Error: The current light type already is a point light!");
						}
						else
						{
							if (type == MeshType::spot_light)
							{
								GameObjectManager::RemoveSpotlight(Select::selectedObj);
							}
							else if (type == MeshType::directional_light)
							{
								GameObjectManager::SetDirectionalLight(nullptr);
							}

							float vertices[] =
							{
								//edges of the cube
								-0.5f, -0.5f, -0.5f,
								 0.5f, -0.5f, -0.5f,

								 0.5f, -0.5f, -0.5f,
								 0.5f,  0.5f, -0.5f,

								 0.5f,  0.5f, -0.5f,
								-0.5f,  0.5f, -0.5f,

								-0.5f,  0.5f, -0.5f,
								-0.5f, -0.5f, -0.5f,

								-0.5f, -0.5f,  0.5f,
								 0.5f, -0.5f,  0.5f,

								 0.5f, -0.5f,  0.5f,
								 0.5f,  0.5f,  0.5f,

								 0.5f,  0.5f,  0.5f,
								-0.5f,  0.5f,  0.5f,

								-0.5f,  0.5f,  0.5f,
								-0.5f, -0.5f,  0.5f,

								//connecting edges
								-0.5f, -0.5f, -0.5f,
								-0.5f, -0.5f,  0.5f,

								 0.5f, -0.5f, -0.5f,
								 0.5f, -0.5f,  0.5f,

								 0.5f,  0.5f, -0.5f,
								 0.5f,  0.5f,  0.5f,

								-0.5f,  0.5f, -0.5f,
								-0.5f,  0.5f,  0.5f,
							};

							GLuint vao, vbo, ebo;

							glGenVertexArrays(1, &vao);
							glGenBuffers(1, &vbo);
							glBindVertexArray(vao);
							glBindBuffer(GL_ARRAY_BUFFER, vbo);
							glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

							glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
							glEnableVertexAttribArray(0);

							glBindVertexArray(0);

							auto mesh = Select::selectedObj->AddComponent<MeshComponent>(
								true,
								MeshType::point_light,
								vao,
								vbo,
								ebo);

							string vert = (path(Engine::filesPath) / "shaders" / "Basic_model.vert").string();
							string frag = (path(Engine::filesPath) / "shaders" / "Basic.frag").string();

							if (!exists(vert)
								|| !exists(frag))
							{
								Engine::CreateErrorPopup("One of the shader paths for point light is invalid!");
							}

							Shader pointLightShader = Shader::LoadShader(vert, frag);

							auto mat = Select::selectedObj->AddComponent<MaterialComponent>();
							mat->AddShader(vert, frag, pointLightShader);

							auto pointLight = Select::selectedObj->AddComponent<LightComponent>(
								vec3(1),
								1.0f,
								1.0f);

							string billboardDiffTexture = (path(Engine::filesPath) / "icons" / "pointLight.png").string();
							auto billboard = Billboard::InitializeBillboard(
								oldBillboardPos,
								vec3(0),
								vec3(1),
								billboardDiffTexture,
								oldBillboardID,
								true);

							billboard->SetParentBillboardHolder(Select::selectedObj);
							Select::selectedObj->SetChildBillboard(billboard);

							Select::selectedObj->SetTxtFilePath(oldTxtPath);

							GameObjectManager::AddPointLight(Select::selectedObj);

							GameObjectManager::DestroyGameObject(oldBillboard, false);
						}
					}
					else if (value == "Spotlight")
					{
						if (type == MeshType::spot_light)
						{
							ConsoleManager::WriteConsoleMessage(
								ConsoleCaller::INPUT,
								ConsoleType::EXCEPTION,
								"Error: The current light type already is a spotlight!");
						}
						else
						{
							if (type == MeshType::point_light)
							{
								GameObjectManager::RemovePointLight(Select::selectedObj);
							}
							else if (type == MeshType::directional_light)
							{
								GameObjectManager::SetDirectionalLight(nullptr);
							}

							float vertices[] =
							{
								//four corner edges
								0.0f,  0.5f,  0.0f,
							   -0.5f, -0.5f, -0.5f,

								0.0f,  0.5f,  0.0f,
								0.5f, -0.5f, -0.5f,

								0.0f,  0.5f,  0.0f,
							   -0.5f, -0.5f,  0.5f,

								0.0f,  0.5f,  0.0f,
								0.5f, -0.5f,  0.5f,

								//four bottom edges
								0.5f, -0.5f,  0.5f,
							   -0.5f, -0.5f,  0.5f,

								0.5f, -0.5f, -0.5f,
							   -0.5f, -0.5f, -0.5f,

							   -0.5f, -0.5f, -0.5f,
							   -0.5f, -0.5f,  0.5f,

								0.5f, -0.5f, -0.5f,
								0.5f, -0.5f,  0.5f
							};

							GLuint vao, vbo, ebo;

							glGenVertexArrays(1, &vao);
							glGenBuffers(1, &vbo);
							glBindVertexArray(vao);
							glBindBuffer(GL_ARRAY_BUFFER, vbo);
							glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

							glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
							glEnableVertexAttribArray(0);

							glBindVertexArray(0);

							auto mesh = obj->AddComponent<MeshComponent>(
								true,
								MeshType::spot_light,
								vao,
								vbo,
								ebo);

							string vert = (path(Engine::filesPath) / "shaders" / "Basic_model.vert").string();
							string frag = (path(Engine::filesPath) / "shaders" / "Basic.frag").string();

							if (!exists(vert)
								|| !exists(frag))
							{
								Engine::CreateErrorPopup("One of the shader paths for spotlight is invalid!");
							}

							Shader spotlightShader = Shader::LoadShader(
								(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
								(path(Engine::filesPath) / "shaders" / "Basic.frag").string());

							auto mat = obj->AddComponent<MaterialComponent>();
							mat->AddShader(
								(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
								(path(Engine::filesPath) / "shaders" / "Basic.frag").string(),
								spotlightShader);

							auto spotlight = obj->AddComponent<LightComponent>(
								vec3(1),
								1.0f,
								1.0f,
								12.5f,
								17.5f);

							string billboardDiffTexture = (path(Engine::filesPath) / "icons" / "spotLight.png").string();
							shared_ptr<GameObject> billboard = Billboard::InitializeBillboard(
								oldBillboardPos,
								vec3(0),
								vec3(1),
								billboardDiffTexture,
								oldBillboardID,
								true);

							billboard->SetParentBillboardHolder(obj);
							obj->SetChildBillboard(billboard);

							obj->SetTxtFilePath(oldTxtPath);

							GameObjectManager::AddSpotLight(Select::selectedObj);

							GameObjectManager::DestroyGameObject(oldBillboard, false);
						}
					}
					else if (value == "Directional light")
					{
						if (GameObjectManager::GetDirectionalLight() != nullptr)
						{
							ConsoleManager::WriteConsoleMessage(
								ConsoleCaller::INPUT,
								ConsoleType::EXCEPTION,
								"Error: A directional light already exists in this scene!");
						}
						else
						{
							if (type == MeshType::directional_light)
							{
								ConsoleManager::WriteConsoleMessage(
									ConsoleCaller::INPUT,
									ConsoleType::EXCEPTION,
									"Error: The current light type already is a directional light!");
							}
							else
							{
								if (type == MeshType::point_light)
								{
									GameObjectManager::RemovePointLight(Select::selectedObj);
								}
								else if (type == MeshType::spot_light)
								{
									GameObjectManager::RemoveSpotlight(Select::selectedObj);
								}

								float vertices[] =
								{
									//four corner edges
									0.0f,  0.5f,  0.0f,
								   -0.5f, -0.5f, -0.5f,

									0.0f,  0.5f,  0.0f,
									0.5f, -0.5f, -0.5f,

									0.0f,  0.5f,  0.0f,
								   -0.5f, -0.5f,  0.5f,

									0.0f,  0.5f,  0.0f,
									0.5f, -0.5f,  0.5f,

									//four bottom edges
									0.5f, -0.5f,  0.5f,
								   -0.5f, -0.5f,  0.5f,

									0.5f, -0.5f, -0.5f,
								   -0.5f, -0.5f, -0.5f,

								   -0.5f, -0.5f, -0.5f,
								   -0.5f, -0.5f,  0.5f,

									0.5f, -0.5f, -0.5f,
									0.5f, -0.5f,  0.5f
								};

								GLuint vao, vbo, ebo;

								glGenVertexArrays(1, &vao);
								glGenBuffers(1, &vbo);
								glBindVertexArray(vao);
								glBindBuffer(GL_ARRAY_BUFFER, vbo);
								glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

								glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
								glEnableVertexAttribArray(0);

								glBindVertexArray(0);

								auto mesh = obj->AddComponent<MeshComponent>(
									true,
									MeshType::directional_light,
									vao,
									vbo,
									ebo);

								string vert = (path(Engine::filesPath) / "shaders" / "Basic_model.vert").string();
								string frag = (path(Engine::filesPath) / "shaders" / "Basic.frag").string();

								if (!exists(vert)
									|| !exists(frag))
								{
									Engine::CreateErrorPopup("One of the shader paths for spotlight is invalid!");
								}

								Shader directionalLightShader = Shader::LoadShader(vert, frag);

								auto mat = obj->AddComponent<MaterialComponent>();
								mat->AddShader(vert, frag, directionalLightShader);

								auto dirlight = obj->AddComponent<LightComponent>(
									vec3(1),
									1.0f);

								string billboardDiffTexture = (path(Engine::filesPath) / "icons" / "directionalLight.png").string();
								shared_ptr<GameObject> billboard = Billboard::InitializeBillboard(
									oldBillboardPos,
									vec3(0),
									vec3(1),
									billboardDiffTexture,
									oldBillboardID,
									true);

								billboard->SetParentBillboardHolder(obj);
								obj->SetChildBillboard(billboard);

								obj->SetTxtFilePath(oldTxtPath);

								GameObjectManager::SetDirectionalLight(Select::selectedObj);

								GameObjectManager::DestroyGameObject(oldBillboard, false);
							}
						}
					}

					currentItem = -1;
					break;
				}
			}
			ImGui::EndCombo();
		}
	}
}
#endif