//Copyright(C) 2025 Lost Empire Entertainment
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
#include "audioplayercomponent.hpp"
#include "audio.hpp"
#include "empty.hpp"

using std::cout;
using std::endl;
using std::to_string;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::filesystem::is_regular_file;
using std::filesystem::exists;
using std::stof;

using Graphics::Render;
using Core::Select;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Components::TransformComponent;
using Graphics::Components::MeshComponent;
using Graphics::Components::MaterialComponent;
using Graphics::Components::LightComponent;
using Graphics::Components::AudioPlayerComponent;
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
using Graphics::Shape::Empty;
using Core::Audio;

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
			if (Select::selectedObj) AddComponent();

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				ConfigFile::SetValue("gui_inspector", "0");
			}

			if (Select::isObjectSelected)
			{
				auto mesh = Select::selectedObj->GetComponent<MeshComponent>();
				auto mat = Select::selectedObj->GetComponent<MaterialComponent>();
				auto light = Select::selectedObj->GetComponent<LightComponent>();
				auto audioPlayer = Select::selectedObj->GetComponent<AudioPlayerComponent>();

				Component_GameObject();
				Component_Transform();
				if (mesh) Component_Mesh();
				if (mat) Component_Material();
				if (light) Component_Light();
				if (audioPlayer) Component_AudioPlayer();
			}

			ImGui::End();
		}
	}

	void GUIInspector::AddComponent()
	{
		const char* items[] = { "Light", "Audio player"};
		static int currentItem = -1;

		const char* currentLabel = (currentItem >= 0) ? items[currentItem] : "Add component";

		if (ImGui::BeginCombo("##AddComponentDropdown", currentLabel))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				if (ImGui::Selectable(items[i], currentItem == i))
				{
					currentItem = i;
					if (strcmp(items[i], "Light") == 0)
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
							if (Select::selectedObj->GetComponent<MeshComponent>()->GetMeshType()
								== MeshComponent::MeshType::model)
							{
								ConsoleManager::WriteConsoleMessage(
									ConsoleCaller::INPUT,
									ConsoleType::EXCEPTION,
									"Error: " + Select::selectedObj->GetName() + " cannot be given a light component because it is a model! Remove the model first and then attach the light component.");
							}
							else
							{
								auto pointLight = Select::selectedObj->AddComponent<LightComponent>(
									vec3(1),
									1.0f,
									1.0f);
							}
						}
					}
					else if (strcmp(items[i], "Audio player") == 0)
					{
						auto existingAudioPlayer = Select::selectedObj->GetComponent<AudioPlayerComponent>();
						if (existingAudioPlayer)
						{
							ConsoleManager::WriteConsoleMessage(
								ConsoleCaller::INPUT,
								ConsoleType::EXCEPTION,
								"Error: " + Select::selectedObj->GetName() + " already has a audio player component!");
						}
						else Select::selectedObj->AddComponent<AudioPlayerComponent>();
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
					if (mesh->GetMeshType() == MeshComponent::MeshType::model)
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

		vec3 pos = obj->GetComponent<TransformComponent>()->GetPosition();
		ImGui::Text("Position");
		if (ImGui::DragFloat3("##objPos", value_ptr(pos), 0.01f))
		{
			obj->GetComponent<TransformComponent>()->SetPosition(pos);
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		vec3 rot = obj->GetComponent<TransformComponent>()->GetRotation();
		ImGui::Text("Rotation");
		if (ImGui::DragFloat3("##objRot", value_ptr(rot), 0.1f))
		{
			obj->GetComponent<TransformComponent>()->SetRotation(rot);
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		if (rot.x > 359.99f
			|| rot.x < -359.99f)
		{
			rot.x = 0.0f;
			obj->GetComponent<TransformComponent>()->SetRotation(rot);
		}
		if (rot.y > 359.99f
			|| rot.y < -359.99f)
		{
			rot.y = 0.0f;
			obj->GetComponent<TransformComponent>()->SetRotation(rot);
		}
		if (rot.z > 359.99f
			|| rot.z < -359.99f)
		{
			rot.z = 0.0f;
			obj->GetComponent<TransformComponent>()->SetRotation(rot);
		}

		vec3 scale = obj->GetComponent<TransformComponent>()->GetScale();
		ImGui::Text("Scale");
		if (ImGui::DragFloat3("##objScale", value_ptr(scale), 0.01f))
		{
			obj->GetComponent<TransformComponent>()->SetScale(scale);
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		if (scale.x < 0.0f)
		{
			scale.x = 0.0f;
			obj->GetComponent<TransformComponent>()->SetScale(scale);
		}
		if (scale.y < 0.0f)
		{
			scale.y = 0.0f;
			obj->GetComponent<TransformComponent>()->SetScale(scale);
		}
		if (scale.z < 0.0f)
		{
			scale.z = 0.0f;
			obj->GetComponent<TransformComponent>()->SetScale(scale);
		}

		ImGui::EndChild();
	}

	void GUIInspector::Component_Mesh()
	{
		auto& obj = Select::selectedObj;

		auto mesh = obj->GetComponent<MeshComponent>();

		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("Mesh", ImVec2(ImGui::GetWindowWidth() - 20, 125.0f), true, childWindowFlags);

		ImGui::Text("Mesh");

		ImGui::Separator();

		//cannot add meshes or remove them from light sources
		if (mesh->GetMeshType() == MeshComponent::MeshType::empty
			|| mesh->GetMeshType() == MeshComponent::MeshType::model)
		{
			ImGui::Text("Model");

			//assign model
			string model_assign = "Add##model_assign";
			if (ImGui::Button(model_assign.c_str()))
			{
				string modelsFolder = (path(Engine::projectPath) / "models").string();
				if (!exists(modelsFolder)) File::CreateNewFolder(modelsFolder);

				GUIProjectItemsList::obj = obj;
				GUIProjectItemsList::type = GUIProjectItemsList::Type::GameobjectModel;
				GUIProjectItemsList::renderProjectItemsList = true;
			}

			if (mesh->GetMeshType() != MeshComponent::MeshType::empty)
			{
				//remove model
				string model_remove = "Remove##model_remove";
				ImGui::SameLine();
				if (ImGui::Button(model_remove.c_str()))
				{
					string txtFile = (path(Engine::projectPath) / obj->GetTxtFilePath()).string();
					string txtFileParent = path(txtFile).parent_path().string();

					string name = obj->GetName();
					unsigned int ID = obj->GetID();
					vec3 pos = obj->GetComponent<TransformComponent>()->GetPosition();
					vec3 rot = obj->GetComponent<TransformComponent>()->GetRotation();
					vec3 scale = obj->GetComponent<TransformComponent>()->GetScale();

					bool is3D{};
					float maxRange{};
					float minRange{};
					float volume{};
					string audioFilePath{};
					auto apc = obj->GetComponent<AudioPlayerComponent>();
					if (apc)
					{
						is3D = apc->Is3D();
						maxRange = apc->GetMaxRange();
						minRange = apc->GetMinRange();
						volume = apc->GetVolume();
						audioFilePath = apc->GetName();
					}

					GameObjectManager::DestroyGameObject(obj, false);

					File::CreateNewFolder(txtFileParent);
					auto empty = Empty::InitializeEmpty(
						pos, 
						rot, 
						scale,
						"",
						name,
						ID,
						true);
					if (audioFilePath != "")
					{
						auto emptyAPC = empty->AddComponent<AudioPlayerComponent>();
						
						emptyAPC->Set3DState(is3D);
						emptyAPC->SetMaxRange(maxRange);
						emptyAPC->SetMinRange(minRange);
						emptyAPC->SetVolume(volume);
						emptyAPC->SetName(audioFilePath);
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
		MeshComponent::MeshType objType = mesh->GetMeshType();

		float height = objType == MeshComponent::MeshType::model ? 250.0f : 150.0f;

		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("Material", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags);
		
		ImGui::Text("Material");

		ImGui::Separator();

		if (objType == MeshType::model)
		{
			const string& diffTexture = mat->GetTextureName(MaterialComponent::TextureType::diffuse);
			const string& specTexture = mat->GetTextureName(MaterialComponent::TextureType::specular);

			//assign diffuse texture
			ImGui::Text("Diffuse texture");
			ImGui::SameLine(ImGui::GetWindowWidth() - 225.0f);
			path diff_texturePath = path(
				Engine::filesPath
				+ mat->GetTextureName(MaterialComponent::TextureType::diffuse));
			string diff_assign = "Add##diff_assign";
			if (ImGui::Button(diff_assign.c_str()))
			{
				GUIProjectItemsList::obj = obj;
				GUIProjectItemsList::textureType = MaterialComponent::TextureType::diffuse;
				GUIProjectItemsList::type = GUIProjectItemsList::Type::GameobjectTexture;
				GUIProjectItemsList::renderProjectItemsList = true;
			}

			//reset diffuse texture
			ImGui::SameLine();
			path diff_defaultTexturePath = path(path(Engine::filesPath) / "textures" / "diff_default.png");
			string diff_reset = "Remove##diff_reset";
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

			ImGui::Spacing();

			//assign specular texture
			ImGui::Text("Specular texture");
			ImGui::SameLine(ImGui::GetWindowWidth() - 225.0f);
			path spec_texturePath = path(
				Engine::filesPath
				+ mat->GetTextureName(MaterialComponent::TextureType::specular));
			string spec_assign = "Add##spec_assign";
			if (ImGui::Button(spec_assign.c_str()))
			{
				GUIProjectItemsList::obj = obj;
				GUIProjectItemsList::textureType = MaterialComponent::TextureType::specular;
				GUIProjectItemsList::type = GUIProjectItemsList::Type::GameobjectTexture;
				GUIProjectItemsList::renderProjectItemsList = true;
			}

			//reset specular texture
			ImGui::SameLine();
			path spec_defaultTexturePath = path(path(Engine::filesPath) / "textures" / "spec_default.png");
			string spec_reset = "Remove##spec_reset";
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

			ImGui::Text("Toggle transparency");
			bool isTransparent = mat->IsTransparent();
			string transparentButtonName = isTransparent ? "Disable" : "Enable";
			if (ImGui::Button(transparentButtonName.c_str()))
			{
				mat->SetTransparent(!isTransparent);

				vector<shared_ptr<GameObject>>& opaqueObjects = GameObjectManager::GetOpaqueObjects();
				vector<shared_ptr<GameObject>>& transparentObjects = GameObjectManager::GetTransparentObjects();
				if (mat->IsTransparent())
				{
					auto it = find(opaqueObjects.begin(), opaqueObjects.end(), obj);

					if (it != opaqueObjects.end())
					{
						transparentObjects.push_back(*it);
						opaqueObjects.erase(it);
					}
				}
				else
				{
					auto it = find(transparentObjects.begin(), transparentObjects.end(), obj);

					if (it != transparentObjects.end())
					{
						opaqueObjects.push_back(*it);
						transparentObjects.erase(it);
					}
				}
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
			ImGui::Text("Set transparency strength");
			float transparencyValue = mat->GetTransparentValue();
			if (ImGui::DragFloat("##transparencyValue", &transparencyValue, 0.001f, 0.0f, 1.0f))
			{
				mat->SetTransparentValue(transparencyValue);
				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
		}

		ImGui::EndChild();
	}

	void GUIInspector::Component_Light()
	{
		auto& obj = Select::selectedObj;

		auto mesh = obj->GetComponent<MeshComponent>();
		auto light = obj->GetComponent<LightComponent>();

		float height = mesh->GetMeshType() == MeshType::spot_light ? 400 : 270;

		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("Light", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags);
		
		ImGui::Text("Light");

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
		if (ImGui::Button("X"))
		{
			obj->RemoveComponent<LightComponent>();
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

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
					vec3 oldPos = Select::selectedObj->GetComponent<TransformComponent>()->GetPosition();
					vec3 oldRot = Select::selectedObj->GetComponent<TransformComponent>()->GetRotation();
					vec3 oldScale = Select::selectedObj->GetComponent<TransformComponent>()->GetScale();

					auto& obj = Select::selectedObj;
					string oldTxtPath = obj->GetTxtFilePath();
					auto& oldBillboard = obj->GetChildBillboard();
					vec3 oldBillboardPos = obj->GetComponent<TransformComponent>()->GetPosition();
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

	void GUIInspector::Component_AudioPlayer()
	{
		auto& obj = Select::selectedObj;
		auto audioPlayer = obj->GetComponent<AudioPlayerComponent>();

		float height = 300.0f;

		ImGuiChildFlags childWindowFlags{};

		ImGui::BeginChild("Audio player", ImVec2(ImGui::GetWindowWidth() - 20, height), true, childWindowFlags);

		ImGui::Text("Audio player");

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
		if (ImGui::Button("X"))
		{
			obj->RemoveComponent<LightComponent>();
			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		ImGui::Separator();

		bool isPlaying = audioPlayer->IsPlaying();
		string playButtonName = isPlaying ? "Stop" : "Play";
		if (ImGui::Button(playButtonName.c_str()))
		{
			string audioFile = audioPlayer->GetName();
			if (audioFile == "")
			{
				ConsoleManager::WriteConsoleMessage(
					ConsoleCaller::INPUT,
					ConsoleType::EXCEPTION,
					"Error: Cannot play because no audio has been assigned!'\n");
			}
			else
			{
				audioPlayer->SetPlayState(!isPlaying);
				bool newPlayState = audioPlayer->IsPlaying();
				if (newPlayState)
				{
					Audio::Play(audioFile);

					ConsoleManager::WriteConsoleMessage(
						ConsoleCaller::INPUT,
						ConsoleType::DEBUG,
						"Playing audio file '" + audioFile + "'.\n");
				}
				else
				{
					Audio::Stop(audioFile);
					audioPlayer->SetPauseState(false);

					ConsoleManager::WriteConsoleMessage(
						ConsoleCaller::INPUT,
						ConsoleType::DEBUG,
						"Stopped audio file '" + audioFile + "'.\n");
				}
			}
		}

		if (isPlaying)
		{
			bool isPaused = audioPlayer->IsPaused();
			string pauseButtonName = isPaused ? "Continue" : "Pause";
			if (ImGui::Button(pauseButtonName.c_str()))
			{
				audioPlayer->SetPauseState(!isPaused);

				string audioFile = audioPlayer->GetName();
				bool newPauseState = audioPlayer->IsPaused();
				if (newPauseState)
				{
					Audio::Pause(audioFile);

					ConsoleManager::WriteConsoleMessage(
						ConsoleCaller::INPUT,
						ConsoleType::DEBUG,
						"Paused audio file '" + audioFile + "'.\n");
				}
				else
				{
					Audio::Continue(audioFile);

					ConsoleManager::WriteConsoleMessage(
						ConsoleCaller::INPUT,
						ConsoleType::DEBUG,
						"Continued audio file '" + audioFile + "'.\n");
				}
			}
		}

		bool is3D = audioPlayer->Is3D();
		if (ImGui::Checkbox("Toggle 3D audio", &is3D))
		{
			audioPlayer->Set3DState(is3D);

			string audioFile = path(audioPlayer->GetName()).filename().string();

			bool new3DState = audioPlayer->Is3D();
			if (is3D)
			{
				ConsoleManager::WriteConsoleMessage(
					ConsoleCaller::INPUT,
					ConsoleType::DEBUG,
					"Set audio file '" + audioFile + "' to 3D.\n");
			}
			else
			{
				ConsoleManager::WriteConsoleMessage(
					ConsoleCaller::INPUT,
					ConsoleType::DEBUG,
					"Set audio file '" + audioFile + "' to 2D.\n");
			}

			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		float currVolume = audioPlayer->GetVolume();
		if (ImGui::SliderFloat("Volume", &currVolume, 0.0f, 100.0f))
		{
			audioPlayer->SetVolume(currVolume);

			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}

		if (audioPlayer->Is3D())
		{
			float currMinRange = audioPlayer->GetMinRange();
			float maxMinRange = audioPlayer->GetMaxRange() - 0.1f;
			if (ImGui::DragFloat("Min range", &currMinRange, 0.1f, 0.01f, maxMinRange, "%.2f"))
			{
				audioPlayer->SetMinRange(currMinRange);

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}

			float currMaxRange = audioPlayer->GetMaxRange();
			float minMaxRange = audioPlayer->GetMinRange() + 0.1f;
			if (ImGui::DragFloat("Max range", &currMaxRange, 0.1f, minMaxRange, 100.0f, "%.2f"))
			{
				audioPlayer->SetMaxRange(currMaxRange);

				if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
			}
		}

		if (ImGui::Button("Set path"))
		{
			string audioFolder = (path(Engine::projectPath) / "audio").string();
			if (!exists(audioFolder)) File::CreateNewFolder(audioFolder);

			GUIProjectItemsList::obj = obj;
			GUIProjectItemsList::type = GUIProjectItemsList::Type::Audio;
			GUIProjectItemsList::renderProjectItemsList = true;

			if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		}
		if (ImGui::IsItemHovered())
		{
			string audioFileName = "Audio file: " + obj->GetComponent<AudioPlayerComponent>()->GetName();
			ImGui::BeginTooltip();
			ImGui::Text(audioFileName.c_str());
			ImGui::EndTooltip();
		}

		ImGui::EndChild();
	}
}
#endif