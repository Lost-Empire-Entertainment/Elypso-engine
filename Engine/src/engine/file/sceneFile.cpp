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
		string projectPath = path(Engine::filesPath).parent_path().string() + "/project/project.txt";
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

		currentScenePath = targetScene;
	}


	void SceneFile::CreateScene()
	{
		int highestFolderNumber = 1;
		string parentPath = path(Engine::filesPath).parent_path().string();
		string projectsPath = parentPath + "/project";
		string newFolderPath = projectsPath + "/Scene";

		for (const auto& entry : directory_iterator(projectsPath))
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

		currentScenePath = newFolderPath + "/Scene.txt";

		ofstream sceneFile(currentScenePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Error: Couldn't open scene file '" + currentScenePath + "'!\n");
			return;
		}

		sceneFile.close();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"\nSuccessfully created new scene '" + currentScenePath + "'!\n");

		LoadScene(currentScenePath);
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

		if (Engine::gamePath == "")
		{
			string parentPath = path(Engine::filesPath).parent_path().string();
			string projectFilePath = parentPath + "/project/project.txt";

			ifstream projectFile(projectFilePath);
			if (!projectFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Couldn't read from project file '" + projectFilePath + "'!\n");
				return;
			}

			string line;
			while (getline(projectFile, line))
			{
				if (line.find("game: ") != string::npos)
				{
					line.erase(0, 6);
					Engine::gamePath = line;
					break;
				}
			}
			projectFile.close();

			string gameExePath = Engine::gamePath + "/build/Release/Game.exe\n";
			gameExePath = String::CharReplace(gameExePath, '/', '\\');

			string gameParentPath = path(gameExePath).parent_path().string();
			gameParentPath = String::CharReplace(gameParentPath, '/', '\\');
		}

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

		ifstream sceneFile(scenePath);
		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Failed to open scene file '" + scenePath + "'!\n\n");
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

				if (type == "id")
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

		currentScenePath = scenePath;

		if (unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully loaded scenePath '" + currentScenePath + "'!\n");
	}

	void SceneFile::LoadGameObject(const map<string, string> obj)
	{
		//
		// VALUES USED FOR ALL GAMEOBJECTS
		//

		unsigned int id = stoul(obj.at("id"));
		string name = obj.at("name");
		auto meshType = magic_enum::enum_cast<Mesh::MeshType>(obj.at("type"));

		vec3 pos{};
		if (obj.count("position"))
		{
			vector<string> vect = String::Split(obj.at("position"), ',');
			pos = String::StringToVec3(vect);
		}

		vec3 rot{};
		if (obj.count("rotation"))
		{
			vector<string> vect = String::Split(obj.at("rotation"), ',');
			rot = String::StringToVec3(vect);
		}

		vec3 scale{};
		if (obj.count("scale"))
		{
			vector<string> vect = String::Split(obj.at("scale"), ',');
			scale = String::StringToVec3(vect);
		}

		vector<string> shaders = String::Split(obj.at("shaders"), ',');

		//
		// EXTRA VALUES
		//

		vector<string> textures;
		if (obj.count("textures")) textures = String::Split(obj.at("textures"), ',');

		float shininess = 32;
		if (obj.count("shininess")) shininess = stof(obj.at("shininess"));

		vec3 diffuse{};
		if (obj.count("diffuse"))
		{
			vector<string> vect = String::Split(obj.at("diffuse"), ',');
			diffuse = String::StringToVec3(vect);
		}

		float intensity;
		if (obj.count("intensity")) intensity = stof(obj.at("intensity"));

		float distance;
		if (obj.count("distance")) distance = stof(obj.at("distance"));

		float innerAngle;
		if (obj.count("inner angle")) innerAngle = stof(obj.at("inner angle"));

		float outerAngle;
		if (obj.count("outer angle")) outerAngle = stof(obj.at("outer angle"));

		string modelPath;
		if (obj.count("model path")) modelPath = obj.at("model path");


		//
		// ATTACHED BILLBOARD VALUES
		//

		string billboardName;
		if (obj.count("billboard name")) billboardName = obj.at("billboard name");

		unsigned int billboardID;
		if (obj.count("billboard id")) billboardID = stoul(obj.at("billboard id"));

		vector<string> billboardShaders{};
		if (obj.count("billboard shaders")) billboardShaders = String::Split(obj.at("billboard shaders"), ',');

		string billboardDiffTexture;
		if (obj.count("billboard texture")) billboardDiffTexture = obj.at("billboard texture");

		float billboardShininess;
		if (obj.count("billboard shininess")) billboardShininess = stof(obj.at("billboard shininess"));

		//
		// CREATE GAMEOBJECTS
		//

		if (meshType == Mesh::MeshType::model)
		{
			Model::Initialize(
				pos,
				rot,
				scale,
				modelPath,
				shaders[0],
				shaders[1],
				textures[0],
				textures[1],
				textures[2],
				textures[3],
				shininess,
				name,
				id);

			GameObject::nextID++;
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

			GameObject::nextID++;
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

			GameObject::nextID++;
		}
	}

	void SceneFile::SaveScene(SaveType saveType, const string& targetLevel)
	{
		if (Engine::gamePath != "")
		{
			string parentPath = path(Engine::filesPath).parent_path().string();
			string projectFilePath = parentPath + "/project/project.txt";

			ifstream projectFile(projectFilePath);
			if (!projectFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Couldn't read from project file '" + projectFilePath + "'!\n");
				return;
			}

			string line;
			string fileContent;
			while (getline(projectFile, line))
			{
				if (line.find("game: ") != string::npos)
				{
					line = "game: " + Engine::gamePath;
				}
				fileContent += line + "\n";
			}
			projectFile.close();

			ofstream outFile(projectFilePath);
			if (!outFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Couldn't write into project file '" + projectFilePath + "'!\n");
				return;
			}
			else
			{
				outFile << fileContent;
				outFile.close();
			}
		}

		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();

		ofstream sceneFile(currentScenePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't write into level file '" + currentScenePath + "'!\n");
			return;
		}

		for (const auto& obj : objects)
		{
			//ignore border gameobject
			//billboards and actiontex

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
			"\nSuccessfully saved scene '" + currentScenePath + "'!\n");

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

	void SceneFile::ExportProjectFiles()
	{
		for (const auto& entry : directory_iterator(currentProjectPath))
		{
			File::DeleteFileOrfolder(entry);
		}

		string parentPath = path(Engine::filesPath).parent_path().string();
		string projectsPath = parentPath + "/project";
		string projectFilePath = projectsPath + "/project.txt";

		ifstream projectFile(projectFilePath);
		if (!projectFile.is_open())
		{
			Engine::CreateErrorPopup(
				"Project file load error", 
				"Failed to open project file! Shutting down engine");
		}

		string line;
		string fileContent;
		while (getline(projectFile, line))
		{
			if (line.find("scene: ") != string::npos)
			{
				line = "scene: " + currentScenePath;
			}
			fileContent += line + "\n";
		}
		projectFile.close();

		ofstream outFile(projectFilePath);
		if (!outFile.is_open())
		{
			Engine::CreateErrorPopup(
				"Project file load error",
				"Failed to open project file! Shutting down engine");
		}
		else
		{
			outFile << fileContent;
			outFile.close();
		}

		for (const auto& entry : directory_iterator(projectsPath))
		{
			string ending = entry.is_directory() 
				? path(entry).filename().string() 
				: path(entry).stem().string() + path(entry).extension().string();

			File::MoveOrRenameFileOrFolder(entry, currentProjectPath + "\\" + ending, false);
		}
	}
}