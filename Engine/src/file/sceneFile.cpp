//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>

//external
#include "magic_enum.hpp"

//engine
#include "scenefile.hpp"
#include "core.hpp"
#include "gameobject.hpp"
#include "selectobject.hpp"
#include "stringutils.hpp"
#include "render.hpp"
#include "model.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "fileutils.hpp"
#include "console.hpp"

using std::ifstream;
using std::ofstream;
using std::cout;
using std::to_string;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;

using Core::Engine;
using Physics::Select;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Utils::String;
using Graphics::Render;
using Graphics::Shape::Mesh;
using Graphics::Shape::Model;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;
using Utils::File;
using Graphics::Shape::Material;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace EngineFile
{
	void SceneFile::CheckForProjectFile()
	{
		string projectPath = Engine::filesPath + "/project.txt";
		if (!exists(projectPath))
		{
			Engine::CreateErrorPopup("Project file load error", "No project file was found! Shutting down engine");
		}

		string targetScene;
		ifstream projectFile(projectPath);
		if (!projectFile.is_open())
		{
			Engine::CreateErrorPopup("Project file load error", "Failed to open project file! Shutting down engine");
		}

		string line;
		while (getline(projectFile, line))
		{
			if (!line.empty())
			{
				size_t pos_scene = line.find("scene:");
				if (pos_scene != string::npos)
				{
					string removable = "scene: ";
					size_t pos = line.find(removable);
					targetScene = line.erase(pos, removable.length());
				}

				size_t pos_project = line.find("project:");
				if (pos_project != string::npos)
				{
					string removable = "project: ";
					size_t pos = line.find(removable);
					currentProjectPath = line.erase(pos, removable.length());
				}
			}
		}
		projectFile.close();

		if (currentProjectPath.empty()
			|| !exists(currentProjectPath))
		{
			Engine::CreateErrorPopup("Project load error", "Failed to load valid project from project file! Shutting down engine");
		}

		if (targetScene.empty()
			|| !exists(targetScene))
		{
			Engine::CreateErrorPopup("Scene load error", "Failed to load valid scene from project file! Shutting down engine");
		}

		Engine::scenePath = targetScene;
	}


	void SceneFile::CreateScene()
	{
		int highestFolderNumber = 1;
		string parentPath = path(Engine::projectPath).parent_path().string();
		string newFolderPath = parentPath + "/Scene";

		for (const auto& entry : directory_iterator(parentPath))
		{
			path entryPath = entry.path();
			
			if (is_directory(entryPath)
				&& entryPath.stem().string().find("Scene") != string::npos)
			{
				string folderName = entryPath.stem().string();

				size_t pos = folderName.find_first_of('e', folderName.find_first_of('e') + 1);
				string result = folderName.substr(pos + 1);
				if (result != ""
					&& String::CanConvertStringToInt(result))
				{
					int number = stoi(result);
					if (number == highestFolderNumber) highestFolderNumber = ++number;
				}
			}
		}
		newFolderPath = newFolderPath + to_string(highestFolderNumber);
		create_directory(newFolderPath);

		Engine::scenePath = newFolderPath + "/Scene.txt";

		ofstream sceneFile(Engine::scenePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Error: Couldn't open scene file '" + Engine::scenePath + "'!\n");
			return;
		}

		sceneFile.close();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"\nSuccessfully created new scene '" + Engine::scenePath + "'!\n");

		LoadScene(Engine::scenePath);
	}

	void SceneFile::LoadScene(const string& scenePath)
	{
		if (!exists(scenePath))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Tried to load scene '" + scenePath + "' but it doesn't exist!\n");
			return;
		}
		else Engine::scenePath = scenePath;

		//create models folder inside current scene folder if it doesnt exist
		string modelsFolder = path(Engine::scenePath).parent_path().string() + "/models";
		if (!exists(modelsFolder)) File::CreateNewFolder(modelsFolder);

		//create textures folder inside current scene folder if it doesnt exist
		string texturesFolder = path(Engine::scenePath).parent_path().string() + "/textures";
		if (!exists(texturesFolder)) File::CreateNewFolder(texturesFolder);

		Select::isObjectSelected = false;
		Select::selectedObj = nullptr;

		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
		if (objects.size() != 0)
		{
			for (const auto& obj : objects)
			{
				GameObjectManager::DestroyGameObject(obj);
			}
		}

		ifstream sceneFile(Engine::scenePath);
		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Failed to open scene file '" + Engine::scenePath + "'!\n\n");
			return;
		}

		string line;
		map<string, string> obj;
		while (getline(sceneFile, line))
		{
			if (!line.empty()
				&& line.find("=") != string::npos)
			{
				vector<string> splitLine = String::Split(line, '=');
				string type = splitLine[0];
				string value = splitLine[1];

				//remove one space in front of value if it exists
				if (value[0] == ' ') value.erase(0, 1);
				//remove one space in front of each value comma if it exists
				for (size_t i = 0; i < value.length(); i++)
				{
					if (value[i] == ','
						&& i + 1 < value.length()
						&& value[i + 1] == ' ')
					{
						value.erase(i + 1, 1);
					}
				}

				if (type == "dirRotation")
				{
					vector<string> values = String::Split(value, ',');
					Render::directionalDirection = String::StringToVec3(values);
				}
				
				else if (type == "dirDiffuse")
				{
					vector<string> values = String::Split(value, ',');
					Render::directionalDiffuse = String::StringToVec3(values);
				}

				else if (type == "dirRotation")
				{
					Render::directionalIntensity = stof(value);
				}

				else if (type == "backgroundColor")
				{
					vector<string> values = String::Split(value, ',');
					Render::backgroundColor = String::StringToVec3(values);
				}

				else if (type == "id")
				{
					if (!obj.empty()) LoadGameObject(obj);

					obj.clear();
					obj[type] = value;
				}
				else obj[type] = value;
			}
		}

		sceneFile.close();

		if (!obj.empty()) LoadGameObject(obj);

		RemoveUnusedFiles();

		if (unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully loaded scenePath '" + Engine::scenePath + "'!\n");
	}

	void SceneFile::LoadGameObject(const map<string, string> obj)
	{
		//
		// VALUES USED FOR ALL GAMEOBJECTS
		//

		unsigned int id;
		string name;
		Mesh::MeshType meshType = {};
		vec3 pos{};
		vec3 rot{};
		vec3 scale{};
		vector<string> shaders;

		//
		// EXTRA VALUES
		//

		vector<string> textures;
		float shininess = 32;
		vec3 diffuse{};
		float intensity;
		float distance;
		float innerAngle;
		float outerAngle;
		string modelPath;

		//
		// ATTACHED BILLBOARD VALUES
		//

		string billboardName;
		unsigned int billboardID;
		vector<string> billboardShaders{};
		string billboardDiffTexture;
		float billboardShininess;

		for (const auto& entry : obj)
		{
			//
			// VALUES USED FOR ALL GAMEOBJECTS
			//

			string type = entry.first;
			string value = entry.second;

			if (type == "id") id = stoul(value);
			if (type == "name") name = value;
			if (type == "type") meshType = magic_enum::enum_cast<Mesh::MeshType>(value).value_or(Mesh::MeshType::model);
			if (type == "position")
			{
				vector<string> values = String::Split(value, ',');
				pos = String::StringToVec3(values);
			}
			if (type == "rotation")
			{
				vector<string> values = String::Split(value, ',');
				rot = String::StringToVec3(values);
			}
			if (type == "scale")
			{
				vector<string> values = String::Split(value, ',');
				scale = String::StringToVec3(values);
			}
			if (type == "shaders")
			{
				shaders = String::Split(value, ',');
			}

			//
			// EXTRA VALUES
			//

			
			if (type == "textures") textures = String::Split(value, ',');
			if (type == "shininess") shininess = stof(value);
			if (type == "diffuse")
			{
				vector<string> values = String::Split(value, ',');
				diffuse = String::StringToVec3(values);
			}
			if (type == "intensity") intensity = stof(value);
			if (type == "distance") distance = stof(value);
			if (type == "inner angle") innerAngle = stof(value);
			if (type == "outer angle") outerAngle = stof(value);
			if (type == "model path") modelPath = value;

			//
			// ATTACHED BILLBOARD VALUES
			//

			if (type == "billboard name") billboardName = value;
			if (type == "billboard id") billboardID = stoul(value);
			if (type == "billboard shaders") billboardShaders = String::Split(value, ',');
			if (type == "billboard texture") billboardDiffTexture = value;
			if (type == "billboard shininess") billboardShininess = stof(value);
		}

		//
		// CREATE GAMEOBJECTS
		//
		
		if (meshType == Mesh::MeshType::model)
		{
			string diff_missing = Engine::filesPath + "/textures/diff_missing.png";
			string diffuseTexture = textures[0];
			if (diffuseTexture != "EMPTY"
				&& !exists(diffuseTexture))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Diffuse texture " + diffuseTexture + " for " + name + " not found!\n");
				diffuseTexture = diff_missing;
			}

			string specularTexture = textures[1];
			if (specularTexture != "EMPTY"
				&& !exists(specularTexture))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Specular texture " + specularTexture + " for " + name + " not found!\n");
				specularTexture = diff_missing;
			}

			string normalTexture = textures[2];
			if (normalTexture != "EMPTY"
				&& !exists(normalTexture))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Normal texture " + normalTexture + " for " + name + " not found!\n");
				normalTexture = diff_missing;
			}

			string heightTexture = textures[3];
			if (heightTexture != "EMPTY"
				&& !exists(heightTexture))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Height texture " + heightTexture + " for " + name + " not found!\n");
				heightTexture = diff_missing;
			}

			Model::Initialize(
				pos,
				rot,
				scale,
				modelPath,
				shaders[0],
				shaders[1],
				diffuseTexture,
				specularTexture,
				normalTexture,
				heightTexture,
				shininess,
				name,
				id);

			GameObject::nextID = id + 1;
		}
		else if (meshType == Mesh::MeshType::point_light)
		{
			PointLight::InitializePointLight(
				pos,
				rot,
				scale,
				shaders[0],
				shaders[1],
				diffuse,
				intensity,
				distance,
				name,
				id,
				billboardShaders[0],
				billboardShaders[1],
				billboardDiffTexture,
				billboardShininess,
				billboardName,
				billboardID);

			GameObject::nextID = id + 1;
		}
		else if (meshType == Mesh::MeshType::spot_light)
		{
			SpotLight::InitializeSpotLight(
				pos,
				rot,
				scale,
				shaders[0],
				shaders[1],
				diffuse,
				intensity,
				distance,
				innerAngle,
				outerAngle,
				name,
				id,
				billboardShaders[0],
				billboardShaders[1],
				billboardDiffTexture,
				billboardShininess,
				billboardName,
				billboardID);

			GameObject::nextID = id + 1;
		}
	}

	void SceneFile::SaveScene(SaveType saveType, const string& targetLevel)
	{
		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();

		ofstream sceneFile(Engine::scenePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't write into level file '" + Engine::scenePath + "'!\n");
			return;
		}

		float dirRotX = Render::directionalDirection.x;
		float dirRotY = Render::directionalDirection.y;
		float dirRotZ = Render::directionalDirection.z;
		sceneFile << "dirDirection= " << dirRotX << ", " << dirRotY << ", " << dirRotZ << "\n";

		float dirDiffX = Render::directionalDiffuse.x;
		float dirDiffY = Render::directionalDiffuse.y;
		float dirDiffZ = Render::directionalDiffuse.z;
		sceneFile << "dirDiffuse= " << dirDiffX << ", " << dirDiffY << ", " << dirDiffZ << "\n";

		float dirIntensity = Render::directionalIntensity;
		sceneFile << "dirIntensity= " << Render::directionalIntensity << "\n";

		sceneFile << "\n";

		float bgrR = Render::backgroundColor.r;
		float bgrG = Render::backgroundColor.g;
		float bgrB = Render::backgroundColor.b;
		sceneFile << "backgroundColor= " << bgrR << ", " << bgrG << ", " << bgrB << "\n";

		sceneFile << "\n==================================================\n\n";

		for (const auto& obj : objects)
		{
			Mesh::MeshType type = obj->GetMesh()->GetMeshType();

			if (obj->GetID() != 10000000
				&& obj->GetID() != 10000001
				&& type != Mesh::MeshType::billboard)
			{
				sceneFile << "id= " << to_string(obj->GetID()) << "\n";

				sceneFile << "\n";

				sceneFile << "name= " << obj->GetName() << "\n";

				string type = string(magic_enum::enum_name(obj->GetMesh()->GetMeshType()));
				sceneFile << "type= " << type << "\n";

				//position
				float posX = obj->GetTransform()->GetPosition().x;
				float posY = obj->GetTransform()->GetPosition().y;
				float posZ = obj->GetTransform()->GetPosition().z;
				sceneFile << "position= " << posX << ", " << posY << ", " << posZ << "\n";

				//rotation
				float rotX = obj->GetTransform()->GetRotation().x;
				float rotY = obj->GetTransform()->GetRotation().y;
				float rotZ = obj->GetTransform()->GetRotation().z;
				sceneFile << "rotation= " << rotX << ", " << rotY << ", " << rotZ << "\n";

				//scale
				float scaleX = obj->GetTransform()->GetScale().x;
				float scaleY = obj->GetTransform()->GetScale().y;
				float scaleZ = obj->GetTransform()->GetScale().z;
				sceneFile << "scale= " << scaleX << ", " << scaleY << ", " << scaleZ << "\n";

				sceneFile << "\n";

				//object textures
				Mesh::MeshType meshType = obj->GetMesh()->GetMeshType();
				if (meshType == Mesh::MeshType::model)
				{
					string diffuseTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::diffuse);
					string specularTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::specular);
					string normalTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::normal);
					string heightTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::height);
					sceneFile
						<< "textures= "
						<< diffuseTexture << ", "
						<< specularTexture << ", "
						<< normalTexture << ", "
						<< heightTexture << "\n";
				}

				//shaders
				string vertexShader = obj->GetMaterial()->GetShaderName(0);
				string fragmentShader = obj->GetMaterial()->GetShaderName(1);
				sceneFile << "shaders= " << vertexShader << ", " << fragmentShader << "\n";

				//material variables
				if (meshType == Mesh::MeshType::model)
				{
					sceneFile << "shininess= " << obj->GetBasicShape()->GetShininess() << "\n";

					sceneFile << "model path= " << obj->GetDirectory() << "\n";
				}
				else if (meshType == Mesh::MeshType::point_light)
				{
					float pointDiffuseX = obj->GetPointLight()->GetDiffuse().x;
					float pointDiffuseY = obj->GetPointLight()->GetDiffuse().y;
					float pointDiffuseZ = obj->GetPointLight()->GetDiffuse().z;
					sceneFile << "diffuse= " << pointDiffuseX << ", " << pointDiffuseY << ", " << pointDiffuseZ << "\n";

					sceneFile << "intensity= " << obj->GetPointLight()->GetIntensity() << "\n";

					sceneFile << "distance= " << obj->GetPointLight()->GetDistance() << "\n";
				}
				else if (meshType == Mesh::MeshType::spot_light)
				{
					float spotDiffuseX = obj->GetSpotLight()->GetDiffuse().x;
					float spotDiffuseY = obj->GetSpotLight()->GetDiffuse().y;
					float spotDiffuseZ = obj->GetSpotLight()->GetDiffuse().z;
					sceneFile << "diffuse= " << spotDiffuseX << ", " << spotDiffuseY << ", " << spotDiffuseZ << "\n";

					sceneFile << "intensity= " << obj->GetSpotLight()->GetIntensity() << "\n";

					sceneFile << "distance= " << obj->GetSpotLight()->GetDistance() << "\n";

					sceneFile << "inner angle= " << obj->GetSpotLight()->GetInnerAngle() << "\n";

					sceneFile << "outer angle= " << obj->GetSpotLight()->GetOuterAngle() << "\n";
				}

				//also save billboard data of each light source
				if (meshType == Mesh::MeshType::point_light
					|| meshType == Mesh::MeshType::spot_light)
				{
					sceneFile << "\n";
					sceneFile << "---attatched billboard data---" << "\n";
					sceneFile << "\n";

					sceneFile << "billboard name= " << obj->GetChildBillboard()->GetName() << "\n";

					sceneFile << "billboard id= " << obj->GetChildBillboard()->GetID() << "\n";

					string billboardVertShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(0);
					string billboardFragShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(1);
					sceneFile << "billboard shaders= " << billboardVertShader << ", " << billboardFragShader << "\n";

					sceneFile << "billboard texture= " << obj->GetChildBillboard()->GetMaterial()->GetTextureName(Material::TextureType::diffuse) << "\n";

					sceneFile << "billboard shininess= " << obj->GetChildBillboard()->GetBasicShape()->GetShininess() << "\n";
				}

				sceneFile << "\n";
				sceneFile << "==================================================\n";
				sceneFile << "\n";
			}
		}

		sceneFile.close();

		if (unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"\nSuccessfully saved scene '" + Engine::scenePath + "'!\n");

		switch (saveType)
		{
		case SaveType::sceneSwitch:
			LoadScene(targetLevel);
			break;
		case SaveType::shutDown:
			Engine::Shutdown();
			break;
		}
	}

	void SceneFile::RemoveUnusedFiles()
	{
		//store paths to all files in models folder
		string modelsFolder = path(Engine::scenePath).parent_path().string() + "/models";

		for (const path& model : directory_iterator(modelsFolder))
		{
			models[model.string()] = false;
		}

		ifstream sceneFile(Engine::scenePath);
		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Failed to open scene file '" + Engine::scenePath + "'!\n\n");
			return;
		}

		string line;
		map<string, string> obj;
		while (getline(sceneFile, line))
		{
			if (!line.empty()
				&& line.find("=") != string::npos)
			{
				vector<string> splitLine = String::Split(line, '=');
				string type = splitLine[0];
				string value = splitLine[1];

				//remove one space in front of value if it exists
				if (value[0] == ' ') value.erase(0, 1);
				//remove one space in front of each value comma if it exists
				for (size_t i = 0; i < value.length(); i++)
				{
					if (value[i] == ','
						&& i + 1 < value.length()
						&& value[i + 1] == ' ')
					{
						value.erase(i + 1, 1);
					}
				}

				if (type == "model path")
				{
					//check if the file in the model path is also found in the models map
					for (const auto& pair : models)
					{
						string pairKey = pair.first;
						bool pairValue = pair.second;

						if (pairValue != true
							&& value == pairKey)
						{
							cout << "found existing model\n" << path(value).filename().string() << "\n";
							pairValue = true;
							break;
						}
					}
					break;
				}
			}
		}
		sceneFile.close();

		//delete all files in the models folder that dont also exist in the scene file
		for (const auto& pair : models)
		{
			string pairKey = pair.first;
			bool pairValue = pair.second;

			if (!pairValue)
			{
				cout << pairKey << " does not exist in this scene and will be deleted...\n";
				File::DeleteFileOrfolder(pairKey);
			}
		}
	}
}