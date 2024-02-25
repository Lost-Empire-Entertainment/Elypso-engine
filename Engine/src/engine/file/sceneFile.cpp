//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <filesystem>

//external
#include "magic_enum.hpp"

//engine
#include "sceneFile.hpp"
#include "gameobject.hpp"
#include "cube.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "selectobject.hpp"
#include "console.hpp"
#include "core.hpp"
#include "render.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "shutdown.hpp"

using std::cout;
using std::to_string;
using std::vector;
using std::shared_ptr;
using std::ofstream;
using std::ifstream;
using std::getline;
using std::replace;
using std::filesystem::current_path;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;
using std::filesystem::copy;

using Graphics::Shape::GameObjectManager;
using Graphics::Shape::GameObject;
using Graphics::Shape::Mesh;
using Graphics::Shape::Material;
using Graphics::Shape::Cube;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;
using Graphics::Render;
using Physics::Select;
using Core::Engine;
using Utils::File;
using Utils::String;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace EngineFile
{
	void SceneFile::CheckForProjectFile()
	{
		path engineParentPath = path(Engine::enginePath).parent_path();
		string projectPath = engineParentPath.string() + "/files/project.txt";
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

		if (targetScene.empty()
			|| !exists(targetScene))
		{
			Engine::CreateErrorPopup("Scene load error", "Failed to load valid scene from project file! Shutting down engine");
		}

		if (currentProjectPath.empty()
			|| !exists(currentProjectPath))
		{
			Engine::CreateErrorPopup("Project load error", "Failed to load valid scene from project file! Shutting down engine");
		}

		currentScenePath = targetScene;
	}

	void SceneFile::CreateNewScene(const string& filePath)
	{
		if (exists(filePath))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Tried to create scene '" + filePath + "' but it already exists!\n");
			return;
		}

		ofstream sceneFile(filePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't open scene file '" + filePath + "'!\n");
			return;
		}

		sceneFile.close();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully created new scene '" + filePath + "'!\n");
	}

	void SceneFile::LoadScene(const string& filePath)
	{
		if (!exists(filePath))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Tried to load scene '" + filePath + "' but it doesn't exist!\n");
			return;
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

		ifstream sceneFile(filePath);
		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Failed to open scene file '" + filePath + "'!\n\n");
			return;
		}

		string line;
		map<string, string> obj;
		while (getline(sceneFile, line))
		{
			if (!line.empty()
				&& line.find(":") != string::npos)
			{
				vector<string> splitLine = String::Split(line, ':');
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

		currentScenePath = filePath;

		if (SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully loaded scene '" + currentScenePath + "'!\n");
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

		if (meshType == Mesh::MeshType::cube)
		{
			Cube::InitializeCube(
				pos,
				rot,
				scale,
				shaders[0],
				shaders[1],
				textures[0],
				textures[1],
				shininess,
				name,
				id);
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
		}
	}

	void SceneFile::SaveCurrentScene(SaveType saveType, const string& targetScene)
	{
		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
		if (objects.size() < 2)
		{
			string tempFullScenePath = path(currentScenePath).replace_extension().string() + "_TEMP.txt";

			ofstream sceneFile(tempFullScenePath);

			if (!sceneFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Couldn't open temp scene file '" + tempFullScenePath + "'!\n");
				return;
			}

			sceneFile.close();

			path fsPath = currentScenePath;
			path tempFSPath = tempFullScenePath;
			File::DeleteFileOrfolder(fsPath);
			File::MoveOrRenameFileOrFolder(tempFSPath, fsPath, true);
			currentScenePath = fsPath.string();

			path tempProjectPath = currentProjectPath + "_TEMP";
			for (const auto& entry : directory_iterator(Engine::filesPath))
			{
				path entryPath = entry.path();
				File::CopyFileOrFolder(entryPath, tempProjectPath);
			}
			File::DeleteFileOrfolder(currentProjectPath);
			File::MoveOrRenameFileOrFolder(tempProjectPath, currentProjectPath, true);

			if (SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(false);

			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"Successfully saved scene '" + currentScenePath + "'!\n");

			return;
		}

		string tempFullScenePath = path(currentScenePath).replace_extension().string() + "_TEMP.txt";

		ofstream sceneFile(tempFullScenePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't open temp scene file '" + tempFullScenePath + "'!\n");
			return;
		}

		for (const auto& obj : objects)
		{
			//ignore border gameobject
			//billboards and actiontex
			if (obj->GetID() != 10000000
				&& obj->GetID() != 10000001
				&& obj->GetMesh()->GetMeshType() != Mesh::MeshType::billboard)
			{
				sceneFile << "id: " << to_string(obj->GetID()) << "\n";

				sceneFile << "\n";

				sceneFile << "name: " << obj->GetName() << "\n";

				string type = string(magic_enum::enum_name(obj->GetMesh()->GetMeshType()));
				sceneFile << "type: " << type << "\n";

				//position
				float posX = obj->GetTransform()->GetPosition().x;
				float posY = obj->GetTransform()->GetPosition().y;
				float posZ = obj->GetTransform()->GetPosition().z;
				sceneFile << "position: " << posX << ", " << posY << ", " << posZ << "\n";

				//rotation
				float rotX = obj->GetTransform()->GetRotation().x;
				float rotY = obj->GetTransform()->GetRotation().y;
				float rotZ = obj->GetTransform()->GetRotation().z;
				sceneFile << "rotation: " << rotX << ", " << rotY << ", " << rotZ << "\n";

				//scale
				float scaleX = obj->GetTransform()->GetScale().x;
				float scaleY = obj->GetTransform()->GetScale().y;
				float scaleZ = obj->GetTransform()->GetScale().z;
				sceneFile << "scale: " << scaleX << ", " << scaleY << ", " << scaleZ << "\n";

				//object textures
				Mesh::MeshType meshType = obj->GetMesh()->GetMeshType();
				if (meshType == Mesh::MeshType::cube)
				{
					string diffuseTexture = obj->GetMaterial()->GetTextureName(0);
					string specularTexture = obj->GetMaterial()->GetTextureName(1);
					sceneFile << "textures: " << diffuseTexture << ", " << specularTexture << "\n";
				}

				//shaders
				string vertexShader = obj->GetMaterial()->GetShaderName(0);
				string fragmentShader = obj->GetMaterial()->GetShaderName(1);
				sceneFile << "shaders: " << vertexShader << ", " << fragmentShader << "\n";

				//material variables
				if (meshType == Mesh::MeshType::cube)
				{
					sceneFile << "shininess: " << obj->GetBasicShape()->GetShininess() << "\n";
				}
				else if (meshType == Mesh::MeshType::point_light)
				{
					float pointDiffuseX = obj->GetPointLight()->GetDiffuse().x;
					float pointDiffuseY = obj->GetPointLight()->GetDiffuse().y;
					float pointDiffuseZ = obj->GetPointLight()->GetDiffuse().z;
					sceneFile << "diffuse: " << pointDiffuseX << ", " << pointDiffuseY << ", " << pointDiffuseZ << "\n";

					sceneFile << "intensity: " << obj->GetPointLight()->GetIntensity() << "\n";

					sceneFile << "distance: " << obj->GetPointLight()->GetDistance() << "\n";
				}
				else if (meshType == Mesh::MeshType::spot_light)
				{
					float spotDiffuseX = obj->GetSpotLight()->GetDiffuse().x;
					float spotDiffuseY = obj->GetSpotLight()->GetDiffuse().y;
					float spotDiffuseZ = obj->GetSpotLight()->GetDiffuse().z;
					sceneFile << "diffuse: " << spotDiffuseX << ", " << spotDiffuseY << ", " << spotDiffuseZ << "\n";

					sceneFile << "intensity: " << obj->GetSpotLight()->GetIntensity() << "\n";

					sceneFile << "distance: " << obj->GetSpotLight()->GetDistance() << "\n";

					sceneFile << "inner angle: " << obj->GetSpotLight()->GetInnerAngle() << "\n";

					sceneFile << "outer angle: " << obj->GetSpotLight()->GetOuterAngle() << "\n";
				}

				//also save billboard data of each light source
				if (meshType == Mesh::MeshType::point_light
					|| meshType == Mesh::MeshType::spot_light)
				{
					sceneFile << "\n";
					sceneFile << "---attatched billboard data---" << "\n";
					sceneFile << "\n";

					sceneFile << "billboard name: " << obj->GetChildBillboard()->GetName() << "\n";

					sceneFile << "billboard id: " << obj->GetChildBillboard()->GetID() << "\n";

					string billboardVertShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(0);
					string billboardFragShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(1);
					sceneFile << "billboard shaders: " << billboardVertShader << ", " << billboardFragShader << "\n";

					sceneFile << "billboard texture: " << obj->GetChildBillboard()->GetMaterial()->GetTextureName(0) << "\n";

					sceneFile << "billboard shininess: " << obj->GetChildBillboard()->GetBasicShape()->GetShininess() << "\n";
				}

				sceneFile << "\n";
				sceneFile << "==================================================\n";
				sceneFile << "\n";
			}
		}

		sceneFile.close();

		path fsPath = currentScenePath;
		path tempFSPath = tempFullScenePath;
		File::DeleteFileOrfolder(fsPath);
		File::MoveOrRenameFileOrFolder(tempFSPath, fsPath, true);
		currentScenePath = fsPath.string();

		path tempProjectPath = currentProjectPath + "_TEMP";
		for (const auto& entry : directory_iterator(Engine::filesPath))
		{
			path entryPath = entry.path();
			File::CopyFileOrFolder(entryPath, tempProjectPath);
		}
		File::DeleteFileOrfolder(currentProjectPath);
		File::MoveOrRenameFileOrFolder(tempProjectPath, currentProjectPath, true);

		if (SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully saved scene '" + currentScenePath + "'!\n");

		switch (saveType)
		{
		case SaveType::sceneSwitch:
			LoadScene(targetScene);
			break;
		case SaveType::shutDown:
			Engine::Shutdown();
			break;
		}
	}
}