//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>

//external
#include "glm.hpp"
#include "miniaudio.h"
#include "physicsworld.hpp"

//engine
#include "gameobject.hpp"
#include "importer.hpp"
#include "model.hpp"
#include "empty.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "directionallight.hpp"
#include "billboard.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "console.hpp"
#include "sceneFile.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "meshcomponent.hpp"
#include "audio.hpp"
#include "audioplayercomponent.hpp"
#include "materialcomponent.hpp"
#include "rigidbodycomponent.hpp"
#include "physics.hpp"
#include "audioobject.hpp"
#include "cameraobject.hpp"
#include "cameracomponent.hpp"
#if ENGINE_MODE
#include "selectedobjectaction.hpp"
#include "selectedobjectborder.hpp"
#include "gui_scenewindow.hpp"
#include "gui_settings.hpp"
#endif

using std::cout;
using std::endl;
using std::sort;
using std::to_string;
using std::remove;
using std::dynamic_pointer_cast;
using glm::distance;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::ifstream;
using std::filesystem::exists;

using Core::Select;
using Graphics::Components::MeshComponent;
using Type = Graphics::Components::MeshComponent::MeshType;
using Graphics::Render;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using ConsoleType = Core::ConsoleManager::Type;
using EngineFile::SceneFile;
using Utils::String;
using Utils::File;
using Core::Audio;
using Graphics::Components::AudioPlayerComponent;
using Graphics::Components::MaterialComponent;
using Core::Physics;
using Graphics::Components::RigidBodyComponent;
using KalaKit::PhysicsWorld;
using Graphics::Shape::AudioObject;
using Graphics::Shape::CameraObject;
using Graphics::Components::CameraComponent;
#if ENGINE_MODE
using Graphics::Shape::ActionTex;
using Graphics::Shape::Border;
using Graphics::GUI::GUISceneWindow;
using Graphics::GUI::GUISettings;
#endif

namespace Graphics::Shape
{
	void GameObjectManager::RenderAll(const mat4& view, const mat4& projection)
	{
		bool has3DAudio = false;

		//opaque objects are rendered first
		if (opaqueObjects.size() > 0)
		{
			for (const auto& obj : opaqueObjects)
			{
				if (obj->GetName() == "") obj->SetName(".");

				auto apc = obj->GetComponent<AudioPlayerComponent>();
				if (apc)
				{
					if (apc->IsPlaying()
						&& Audio::HasReachedEnd(apc->GetName(), obj))
					{
						apc->SetPlayState(false);
					}

					if (apc->Is3D())
					{
						has3DAudio = true;

						string apcName = apc->GetName();
						if (Audio::IsImported(apc->GetName(), obj))
						{
							Audio::UpdatePlayerPosition(apcName, obj->GetComponent<TransformComponent>()->GetPosition(), obj);
						}
					}
				}

				auto mesh = obj->GetComponent<MeshComponent>();
				Type type = mesh->GetMeshType();
				switch (type)
				{
				case Type::model:
					if (!obj->GetComponent<MaterialComponent>()->IsTransparent())
					{
						Model::Render(obj, view, projection);
					}
					break;
				case Type::empty:
					Empty::RenderEmpty(obj, view, projection);
					break;
				case Type::audio:
					AudioObject::RenderAudioObject(obj, view, projection);
					break;
				case Type::camera:
					CameraObject::RenderCameraObject(obj, view, projection);
					break;
				case Type::directional_light:
					DirectionalLight::RenderDirectionalLight(obj, view, projection);
					break;
				case Type::point_light:
					PointLight::RenderPointLight(obj, view, projection);
					break;
				case Type::spot_light:
					SpotLight::RenderSpotLight(obj, view, projection);
					break;
				}
			}
		}

		auto tc = Render::activeCamera->GetComponent<TransformComponent>();
		auto cc = Render::activeCamera->GetComponent<CameraComponent>();
		if (has3DAudio)
		{
			vec3 camPos = tc->GetPosition();
			vec3 camFront = cc->GetFront();
			vec3 camUp = cc->GetUp();
			Audio::UpdateListenerPosition(camPos, camFront, camUp);
		}
		else
		{
			vec3 camFront = cc->GetFront();
			vec3 camUp = cc->GetUp();
			Audio::UpdateListenerPosition(vec3(0), camFront, camUp);
		}

#if ENGINE_MODE
		Border::RenderBorder(border, view, projection);
#endif
		//transparent objects are rendered last
		if (transparentObjects.size() > 0)
		{
			vec3 camPos = tc->GetPosition();
			sort(transparentObjects.begin(), transparentObjects.end(),
				[&camPos, &view](const auto& a, const auto& b)
				{
					vec3 objectPositionA = a->template GetComponent<TransformComponent>()->GetPosition();
					vec3 objectPositionB = b->template GetComponent<TransformComponent>()->GetPosition();
					
					float distanceA = length(objectPositionA - camPos);
					float distanceB = length(objectPositionB - camPos);
					return distanceA > distanceB;
				});

			glDepthFunc(GL_ALWAYS);
			glDepthMask(GL_FALSE);
			glDisable(GL_CULL_FACE);
#if ENGINE_MODE
			ActionTex::RenderActionTex(actionTex, view, projection);
#endif
			for (const auto& obj : transparentObjects)
			{
				if (obj->GetName() == "") obj->SetName(".");

				auto mesh = obj->GetComponent<MeshComponent>();
				Type type = mesh->GetMeshType();
				switch (type)
				{
				case Type::billboard:
					Billboard::RenderBillboard(obj, view, projection);
					break;
				case Type::model:
					if (obj->GetComponent<MaterialComponent>()->IsTransparent())
					{
						Model::Render(obj, view, projection);
					}
					break;
				}
			}

			glDepthFunc(GL_LESS);
			glDepthMask(GL_TRUE);
			glEnable(GL_CULL_FACE);
		}
	}

	void GameObjectManager::DestroyGameObject(const shared_ptr<GameObject>& obj, bool localOnly)
	{
		if (obj == nullptr) return;

		string thisName = obj->GetName();

		auto mesh = obj->GetComponent<MeshComponent>();
		Type type = mesh->GetMeshType();

		/*
		//destroy all children if parent is destroyed
		if (obj->GetChildren().size() > 0)
		{
			for (const auto& child : obj->GetChildren())
			{
				GameObjectManager::DestroyGameObject(child, true);
			}
		}
		//remove object from parent children vector
		if (obj->GetParent() != nullptr)
		{
			obj->GetParent()->RemoveChild(obj);
		}
		*/

		switch (type)
		{
		case Type::model:
			if (obj->GetComponent<RigidBodyComponent>() != nullptr)
			{
				PhysicsWorld* physicsWorld = Physics::physicsWorld;
				physicsWorld->RemoveRigidBody(obj->GetComponent<RigidBodyComponent>()->GetHandle());
				obj->RemoveComponent<RigidBodyComponent>();
			}
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			if (!obj->GetComponent<MaterialComponent>()->IsTransparent())
			{
				opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			}
			else
			{
				transparentObjects.erase(std::remove(transparentObjects.begin(), transparentObjects.end(), obj), transparentObjects.end());
			}
			break;
		case Type::empty:
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			break;
		case Type::audio:
		{
			shared_ptr<GameObject> childBillboard = obj->GetChildBillboard();
			obj->RemoveChildBillboard();
			DestroyGameObject(childBillboard, false);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			break;
		}
		case Type::camera:
		{
#if ENGINE_MODE
			//remove player camera name from 
			//settings game player camera slot 
			//because this player camera was destroyed
			if (GUISettings::playerCameraName == obj->GetName())
			{
				GUISettings::playerCameraName = "";
			}
#endif

			shared_ptr<GameObject> childBillboard = obj->GetChildBillboard();
			obj->RemoveChildBillboard();
			DestroyGameObject(childBillboard, false);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			break;
		}
		case Type::point_light:
		{
			shared_ptr<GameObject> childBillboard = obj->GetChildBillboard();
			obj->RemoveChildBillboard();
			DestroyGameObject(childBillboard, false);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			pointLights.erase(std::remove(pointLights.begin(), pointLights.end(), obj), pointLights.end());
			break;
		}
		case Type::spot_light:
		{
			shared_ptr<GameObject> childBillboard = obj->GetChildBillboard();
			obj->RemoveChildBillboard();
			DestroyGameObject(childBillboard, false);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			spotLights.erase(std::remove(spotLights.begin(), spotLights.end(), obj), spotLights.end());
			break;
		}
		case Type::directional_light:
		{
			shared_ptr<GameObject> childBillboard = obj->GetChildBillboard();
			obj->RemoveChildBillboard();
			DestroyGameObject(childBillboard, false);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			directionalLight = nullptr;
			break;
		}
		case Type::billboard:
		{
			obj->SetParentBillboardHolder(nullptr);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			transparentObjects.erase(std::remove(transparentObjects.begin(), transparentObjects.end(), obj), transparentObjects.end());
			billboards.erase(std::remove(billboards.begin(), billboards.end(), obj), billboards.end());
			break;
		}
		}

		//also delete the externally saved folder of this gameobject
		if (!localOnly
			&& Select::selectedObj != nullptr)
		{
			for (const auto& entry : directory_iterator(Engine::currentGameobjectsPath))
			{
				string pathName = path(entry).stem().string();
				if (Select::selectedObj->GetName() == pathName)
				{
					string entryString = path(entry).string();
					File::DeleteFileOrfolder(entryString);
					break;
				}
			}
		}
		Select::selectedObj = nullptr;
		Select::isObjectSelected = false;
#if ENGINE_MODE
		GUISceneWindow::UpdateCounts();
#endif

		if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
		
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			ConsoleType::DEBUG,
			"Deleted gameobject " + thisName + ".\n");
	}

	void GameObjectManager::FindAndDestroyGameObject(const string& objTxtFile, bool localOnly)
	{
		ifstream target(objTxtFile);
		if (!target.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				ConsoleType::EXCEPTION,
				"Error: Failed to open target txt file '" + objTxtFile + "'!\n\n");
			return;
		}

		string name{};
		unsigned int ID{};

		//find and store the name and ID of the object from the object txt file
		string line;
		while (getline(target, line))
		{
			if (!line.empty()
				&& line.find("=") != string::npos)
			{
				vector<string> splitLine = String::Split(line, '=');
				string key = splitLine[0];
				string value = splitLine[1];

				//remove one space in front of value if it exists
				if (value[0] == ' ') value.erase(0, 1);

				if (key == "name") name = value;
				if (key == "id") ID = stoul(value);
			}
		}

		target.close();

		//find and delete the object in the scene based off of the name and ID from its object txt file
		for (const auto& obj : GameObjectManager::GetObjects())
		{
			string objName = obj->GetName();
			unsigned int objID = obj->GetID();

			if (name == objName
				&& ID == objID)
			{
				GameObjectManager::DestroyGameObject(obj, true);

				break;
			}
		}
	}
}
