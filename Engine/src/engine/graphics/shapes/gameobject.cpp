//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>

//external
#include "glm.hpp"

//engine
#include "gameobject.hpp"
#include "importer.hpp"
#include "model.hpp"
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
#if ENGINE_MODE
#include "selectedobjectaction.hpp"
#include "selectedobjectborder.hpp"
#include "gui_scenewindow.hpp"
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
using Graphics::Components::Mesh;
using Type = Graphics::Components::Mesh::MeshType;
using Graphics::Render;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using ConsoleType = Core::ConsoleManager::Type;
using EngineFile::SceneFile;
using Utils::String;
using Utils::File;
#if ENGINE_MODE
using Graphics::Shape::ActionTex;
using Graphics::Shape::Border;
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	void GameObjectManager::RenderAll(const mat4& view, const mat4& projection)
	{
		//opaque objects are rendered first
		if (opaqueObjects.size() > 0)
		{
			for (const auto& obj : opaqueObjects)
			{
				if (obj->GetName() == "") obj->SetName(".");

				Type type = obj->GetComponent<Mesh>()->GetMeshType();
				switch (type)
				{
				case Type::model:
					Model::Render(obj, view, projection);
					break;
				case Type::directional_light:
				case Type::point_light:
				case Type::spot_light:
					obj->Render(view, projection);
					break;
				}
			}
		}

#if ENGINE_MODE
		Border::RenderBorder(border, view, projection);
#endif
		//transparent objects are rendered last
		if (transparentObjects.size() > 0)
		{
			sort(transparentObjects.begin(), transparentObjects.end(),
				[&view](const auto& a, const auto& b)
				{
					//calculate the distance along the viewing direction vector from the camera
					vec3 cameraPosition = vec3(view[3]);
					vec3 objectPositionA = a->GetTransform()->GetPosition();
					vec3 objectPositionB = b->GetTransform()->GetPosition();

					//project object positions onto the viewing direction vector
					float distanceA = dot(objectPositionA - cameraPosition, vec3(view[2]));
					float distanceB = dot(objectPositionB - cameraPosition, vec3(view[2]));

					//sort based on the projected distances
					return distanceA > distanceB; //render from back to front
				});

			glDepthMask(GL_FALSE);
			glDisable(GL_CULL_FACE);
#if ENGINE_MODE
			ActionTex::RenderActionTex(selectedAction, view, projection);
#endif
			for (const auto& obj : transparentObjects)
			{
				if (obj->GetName() == "") obj->SetName(".");

				Type type = obj->GetComponent<Mesh>()->GetMeshType();
				switch (type)
				{
				case Type::billboard:
					Billboard::RenderBillboard(obj, view, projection);
					break;
				}
			}

			glDepthMask(GL_TRUE);
			glEnable(GL_CULL_FACE);
		}
	}

	void GameObjectManager::DestroyGameObject(const shared_ptr<GameObject>& obj, bool localOnly)
	{
		if (obj == nullptr) return;

		string thisName = obj->GetName();

		Type type = obj->GetComponent<Mesh>()->GetMeshType();

		Select::selectedObj = nullptr;
		Select::isObjectSelected = false;

		string txtFilePath = obj->GetTxtFilePath();

		//destroy all children if parent is destroyed
		if (obj->GetChildren().size() > 0)
		{
			for (const auto& child : obj->GetChildren())
			{
				GameObjectManager::DestroyGameObject(child, true);
			}
		}
		//remove object from parent children vector
		/*
		if (obj->GetParent() != nullptr)
		{
			obj->GetParent()->RemoveChild(obj);
		}
		*/

		shared_ptr<GameObject> billboard = nullptr;
		/*
		if (obj->GetChildren().size() > 0)
		{
			for (const auto& child : obj->GetChildren())
			{
				if (child->GetComponent<Mesh>()->GetMeshType() == Mesh::MeshType::billboard)
				{
					billboard = child;
					break;
				}
			}
		}
		*/

		switch (type)
		{
		case Type::model:
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			break;
		case Type::point_light:
			//obj->RemoveChild(billboard);
			DestroyGameObject(billboard, false);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			pointLights.erase(std::remove(pointLights.begin(), pointLights.end(), obj), pointLights.end());
			break;
		case Type::spot_light:
			//obj->RemoveChild(billboard);
			DestroyGameObject(billboard, false);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			spotLights.erase(std::remove(spotLights.begin(), spotLights.end(), obj), spotLights.end());
			break;
		case Type::directional_light:
			//obj->RemoveChild(billboard);
			DestroyGameObject(billboard, false);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			opaqueObjects.erase(std::remove(opaqueObjects.begin(), opaqueObjects.end(), obj), opaqueObjects.end());
			directionalLight = nullptr;
			break;
		case Type::billboard:
			//billboard->SetParent(nullptr);
			objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
			transparentObjects.erase(std::remove(transparentObjects.begin(), transparentObjects.end(), obj), transparentObjects.end());
			billboards.erase(std::remove(billboards.begin(), billboards.end(), obj), billboards.end());
			break;
		}

		//also delete the externally saved folder of this gameobject
		if (!localOnly
			&& obj != nullptr)
		{
			string txtFilePath = (path(Engine::scenePath) / obj->GetTxtFilePath()).string();
			if (exists(txtFilePath))
			{
				string targetFolder;
				if (obj->GetComponent<Mesh>()->GetMeshType() == Mesh::MeshType::model)
				{
					targetFolder = path(txtFilePath).parent_path().parent_path().string();
				}
				else if (obj->GetComponent<Mesh>()->GetMeshType() == Mesh::MeshType::point_light
						 || obj->GetComponent<Mesh>()->GetMeshType() == Mesh::MeshType::spot_light
						 || obj->GetComponent<Mesh>()->GetMeshType() == Mesh::MeshType::directional_light)
				{
					targetFolder = path(txtFilePath).parent_path().string();
				}
				if (exists(targetFolder))
				{
					File::DeleteFileOrfolder(targetFolder);
				}
			}
		}
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