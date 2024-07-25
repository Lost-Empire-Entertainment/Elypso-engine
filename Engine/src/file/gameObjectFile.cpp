//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>

//external
#include "magic_enum.hpp"
#include "glm.hpp"

//engine
#include "gameObjectFile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "model.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"

using std::ifstream;
using std::ofstream;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using glm::vec3;

using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::File;
using Graphics::Shape::Mesh;
using Utils::String;
using Graphics::Shape::Material;
using Graphics::Shape::Model;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;

namespace EngineFile
{
	void GameObjectFile::SaveGameObject(const shared_ptr<GameObject>& obj)
	{
		string gameobjectsFolder = path(Engine::projectPath).parent_path().string() + "\\gameobjects";

		string objectPath = gameobjectsFolder + "\\" + obj->GetName();
		string objectName = path(objectPath).stem().string();

		if (exists(objectPath)
			&& exists(objectPath + "\\settings.txt"))
		{
			File::DeleteFileOrfolder(objectPath + "\\settings.txt");
		}

		string objectFilePath = objectPath + "\\settings.txt";
		ofstream objectFile(objectFilePath);

		if (!objectFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Couldn't write into object file '" + objectFilePath + "'!\n");
			return;
		}

		objectFile << "name= " << obj->GetName() << "\n";

		objectFile << "id= " << obj->GetID() << "\n";

		string type = string(magic_enum::enum_name(obj->GetMesh()->GetMeshType()));
		objectFile << "type= " << type << "\n";

		//position
		float posX = obj->GetTransform()->GetPosition().x;
		float posY = obj->GetTransform()->GetPosition().y;
		float posZ = obj->GetTransform()->GetPosition().z;
		objectFile << "position= " << posX << ", " << posY << ", " << posZ << "\n";

		//rotation
		float rotX = obj->GetTransform()->GetRotation().x;
		float rotY = obj->GetTransform()->GetRotation().y;
		float rotZ = obj->GetTransform()->GetRotation().z;
		objectFile << "rotation= " << rotX << ", " << rotY << ", " << rotZ << "\n";

		//scale
		float scaleX = obj->GetTransform()->GetScale().x;
		float scaleY = obj->GetTransform()->GetScale().y;
		float scaleZ = obj->GetTransform()->GetScale().z;
		objectFile << "scale= " << scaleX << ", " << scaleY << ", " << scaleZ << "\n";

		objectFile << "\n";

		//object textures
		Mesh::MeshType meshType = obj->GetMesh()->GetMeshType();
		if (meshType == Mesh::MeshType::model)
		{
			string diffuseTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::diffuse);
			diffuseTexture = path(diffuseTexture).filename().string();
			if (diffuseTexture == "diff_default.png") diffuseTexture = "DEFAULTDIFF";
			string specularTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::specular);
			specularTexture = path(specularTexture).filename().string();
			if (specularTexture == "spec_default.png") specularTexture = "DEFAULTSPEC";
			string normalTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::normal);
			normalTexture = path(normalTexture).filename().string();
			string heightTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::height);
			heightTexture = path(heightTexture).filename().string();
			objectFile
				<< "textures= "
				<< diffuseTexture << ", "
				<< specularTexture << ", "
				<< normalTexture << ", "
				<< heightTexture << "\n";
		}

		//shaders
		string vertexShader = obj->GetMaterial()->GetShaderName(0);
		vertexShader = path(vertexShader).filename().string();
		string fragmentShader = obj->GetMaterial()->GetShaderName(1);
		fragmentShader = path(fragmentShader).filename().string();
		objectFile << "shaders= " << vertexShader << ", " << fragmentShader << "\n";

		//material variables
		if (meshType == Mesh::MeshType::model)
		{
			objectFile << "shininess= " << obj->GetBasicShape()->GetShininess() << "\n";

			string modelPath = String::CharReplace(obj->GetDirectory(), '/', '\\');
			modelPath = path(modelPath).filename().string();
			objectFile << "model= " << modelPath << "\n";
		}
		else if (meshType == Mesh::MeshType::point_light)
		{
			float pointDiffuseX = obj->GetPointLight()->GetDiffuse().x;
			float pointDiffuseY = obj->GetPointLight()->GetDiffuse().y;
			float pointDiffuseZ = obj->GetPointLight()->GetDiffuse().z;
			objectFile << "diffuse= " << pointDiffuseX << ", " << pointDiffuseY << ", " << pointDiffuseZ << "\n";

			objectFile << "intensity= " << obj->GetPointLight()->GetIntensity() << "\n";

			objectFile << "distance= " << obj->GetPointLight()->GetDistance() << "\n";
		}
		else if (meshType == Mesh::MeshType::spot_light)
		{
			float spotDiffuseX = obj->GetSpotLight()->GetDiffuse().x;
			float spotDiffuseY = obj->GetSpotLight()->GetDiffuse().y;
			float spotDiffuseZ = obj->GetSpotLight()->GetDiffuse().z;
			objectFile << "diffuse= " << spotDiffuseX << ", " << spotDiffuseY << ", " << spotDiffuseZ << "\n";

			objectFile << "intensity= " << obj->GetSpotLight()->GetIntensity() << "\n";

			objectFile << "distance= " << obj->GetSpotLight()->GetDistance() << "\n";

			objectFile << "inner angle= " << obj->GetSpotLight()->GetInnerAngle() << "\n";

			objectFile << "outer angle= " << obj->GetSpotLight()->GetOuterAngle() << "\n";
		}

		//also save billboard data of each light source
		if (meshType == Mesh::MeshType::point_light
			|| meshType == Mesh::MeshType::spot_light)
		{
			objectFile << "\n";
			objectFile << "---attatched billboard data---" << "\n";
			objectFile << "\n";

			objectFile << "billboard name= " << obj->GetChildBillboard()->GetName() << "\n";

			objectFile << "billboard id= " << obj->GetChildBillboard()->GetID() << "\n";

			string billboardVertShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(0);
			billboardVertShader = path(billboardVertShader).filename().string();
			string billboardFragShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(1);
			billboardFragShader = path(billboardFragShader).filename().string();
			objectFile << "billboard shaders= " << billboardVertShader << ", " << billboardFragShader << "\n";

			string billboardTexture = obj->GetChildBillboard()->GetMaterial()->GetTextureName(Material::TextureType::diffuse);
			billboardTexture = path(billboardTexture).filename().string();
			objectFile << "billboard texture= " << billboardTexture << "\n";

			objectFile << "billboard shininess= " << obj->GetChildBillboard()->GetBasicShape()->GetShininess() << "\n";
		}

		objectFile.close();
	}

	void GameObjectFile::LoadGameObjects(const string& targetPath)
	{
		for (const auto& folder : directory_iterator(targetPath))
		{
			string settingsFilePath = targetPath + "\\" + path(folder).stem().string() + "\\settings.txt";

			ifstream settingsFile(settingsFilePath);
			if (!settingsFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Failed to open settings file '" + settingsFilePath + "'!\n\n");
				return;
			}

			map<string, string> data;
			string line;
			while (getline(settingsFile, line))
			{
				if (!line.empty()
					&& line.find("=") != string::npos)
				{
					vector<string> splitLine = String::Split(line, '=');
					string key = splitLine[0];
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

					if (key == "name"
						|| key == "id"
						|| key == "type"
						|| key == "position"
						|| key == "rotation"
						|| key == "scale"
						|| key == "model"
						|| key == "textures"
						|| key == "shaders"
						
						|| key == "billboard name"
						|| key == "billboard id"
						|| key == "billboard texture"
						|| key == "billboard shaders"
						|| key == "billboard shininess"
						
						|| key == "diffuse"
						|| key == "shininess"
						|| key == "intensity"
						|| key == "distance"
						|| key == "innerAngle"
						|| key == "outerAngle")
					{
						data[key] = value;
					}
				}
			}

			settingsFile.close();

			LoadGameObject(data, path(folder).stem().string());
		}
	}

	void GameObjectFile::LoadGameObject(const map<string, string>& data, const string& folderPath)
	{
		string name = "MISSING NAME";
		unsigned int id = 0;
		Mesh::MeshType type = Mesh::MeshType::model;
		vec3 pos = vec3();
		vec3 rot = vec3();
		vec3 scale = vec3();
		string modelPath = "";
		vector<string> textures{};
		vector<string> shaders{};

		string billboardName = "MISSING NAME";
		unsigned int billboardID = 0;
		string billboardDiffTexture = "";
		vector<string> billboardShaders{};
		float billboardShininess = 32.0f;

		vec3 diffuse = vec3();
		float shininess = 32.0f;
		float intensity = 1.0f;
		float distance = 2.0f;
		float innerAngle = 12.0f;
		float outerAngle = 25.0f;

		for (const auto& kvp : data)
		{
			string key = kvp.first;
			string value = kvp.second;

			if (key == "name")
			{
				name = value;
			}
			else if (key == "id") id = stoul(value);
			else if (key == "type")
			{
				auto typeAuto = magic_enum::enum_cast<Mesh::MeshType>(value);
				if (typeAuto.has_value())
				{
					type = typeAuto.value();
				}
			}
			else if (key == "position")
			{
				vector<string> split = String::Split(value, ',');
				vec3 newPos = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
				pos = newPos;
			}
			else if (key == "rotation")
			{
				vector<string> split = String::Split(value, ',');
				vec3 newRot = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
				rot = newRot;
			}
			else if (key == "scale")
			{
				vector<string> split = String::Split(value, ',');
				vec3 newScale = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
				scale = newScale;
			}
			else if (key == "model")
			{
				string fullModelPath = Engine::gameobjectsPath + "\\" + folderPath + "\\" + value;

				if (!exists(fullModelPath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Failed to find model for " + name + " at " + fullModelPath + "! Skipped loading gameobject.\n");
					return;
				}
				else modelPath = fullModelPath;
			}
			else if (key == "textures")
			{
				vector<string> split = String::Split(value, ',');

				string fullTex0Path = split[0] == "DEFAULTDIFF"
					? "DEFAULTDIFF"
					: Engine::gameobjectsPath + "\\" + folderPath + "\\" + split[0];

				if (fullTex0Path == "DEFAULTDIFF")
				{
					fullTex0Path = Engine::filesPath + "\\textures\\diff_default.png";
					textures.push_back(fullTex0Path);
				}
				else if (fullTex0Path != "DEFAULTDIFF"
						 && !exists(fullTex0Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Texture at slot 0 for " + name + " at " + fullTex0Path + " does not exist!\n");
					textures.push_back(Engine::filesPath + "\\textures\\diff_missing.png");
				}
				else textures.push_back(fullTex0Path);

				string fullTex1Path = split[1] == "DEFAULTSPEC"
					? "DEFAULTSPEC"
					: Engine::gameobjectsPath + "\\" + folderPath + "\\" + split[1];
				if (fullTex1Path == "DEFAULTSPEC")
				{
					fullTex1Path = Engine::filesPath + "\\textures\\spec_default.png";
					textures.push_back(fullTex1Path);
				}
				else if (fullTex1Path != "DEFAULTSPEC"
						 && !exists(fullTex1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Texture at slot 1 for " + name + " at " + fullTex1Path + " does not exist!\n");
					textures.push_back("EMPTY");
				}
				else textures.push_back(fullTex1Path);

				string fullTex2Path = split[2] == "EMPTY"
					? "EMPTY"
					: Engine::gameobjectsPath + "\\" + folderPath + "\\" + split[2];
				if (fullTex2Path != "EMPTY"
					&& !exists(fullTex2Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Texture at slot 2 for " + name + " at " + fullTex2Path + " does not exist!\n");
					textures.push_back("EMPTY");
				}
				else textures.push_back(fullTex2Path);

				string fullTex3Path = split[3] == "EMPTY"
					? "EMPTY"
					: Engine::gameobjectsPath + "\\" + folderPath + "\\" + split[3];
				if (fullTex3Path != "EMPTY"
					&& !exists(fullTex3Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Texture at slot 3 for " + name + " at " + fullTex3Path + " does not exist!\n");
					textures.push_back("EMPTY");
				}
				else textures.push_back(fullTex3Path);
			}
			else if (key == "shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"One or more shaders are missing for " + name + " at " + value + "! Skipped loading gameobject.\n");
					return;
				}
				else
				{
					shaders.push_back(fullShader0Path);
					shaders.push_back(fullShader1Path);
				}
			}

			else if (key == "billboard name") billboardName = value;
			else if (key == "billboard id") billboardID = stoul(value);
			else if (key == "billboard texture")
			{
				string fullTexPath = Engine::filesPath + "\\icons\\" + value;

				if (!exists(fullTexPath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Texture is missing for " + name + " at " + fullTexPath + "! Skipped loading billboard.\n");
					return;
				}
				else billboardDiffTexture = fullTexPath;
			}
			else if (key == "billboard shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"One or more shaders are missing for " + name + " at " + value + "! Skipped loading billboard.\n");
					return;
				}
				else
				{
					billboardShaders.push_back(fullShader0Path);
					billboardShaders.push_back(fullShader1Path);
				}
			}
			else if (key == "billboard shininess") billboardShininess = stof(value);

			else if (key == "diffuse")
			{
				vector<string> split = String::Split(value, ',');
				vec3 newDiffuse = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
				diffuse = newDiffuse;
			}
			else if (key == "shininess") shininess = stof(value);
			else if (key == "intensity") intensity = stof(value);
			else if (key == "distance") distance = stof(value);
			else if (key == "innerAngle") innerAngle = stof(value);
			else if (key == "outerAngle") outerAngle = stof(value);
		}

		//
		// LOAD REGULAR GAMEOBJECT
		//

		if (type == Mesh::MeshType::model)
		{
			string diff_missing = Engine::filesPath + "\\textures\\diff_missing.png";
			string diffuseTexture = textures[0];
			if (diffuseTexture != "EMPTY"
				&& !exists(diffuseTexture))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Diffuse texture " + diffuseTexture + " for " + name + " not found!\n");
				diffuseTexture = diff_missing;
			}

			string specularTexture = textures[1];
			if (specularTexture != "EMPTY"
				&& !exists(specularTexture))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Specular texture " + specularTexture + " for " + name + " not found!\n");
				specularTexture = "EMPTY";
			}

			string normalTexture = textures[2];
			if (normalTexture != "EMPTY"
				&& !exists(normalTexture))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Normal texture " + normalTexture + " for " + name + " not found!\n");
				normalTexture = "EMPTY";
			}

			string heightTexture = textures[3];
			if (heightTexture != "EMPTY"
				&& !exists(heightTexture))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Height texture " + heightTexture + " for " + name + " not found!\n");
				heightTexture = "EMPTY";
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

		//
		// LOAD POINT LIGHT
		//

		else if (type == Mesh::MeshType::point_light)
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

		//
		// LOAD SPOTLIGHT
		//

		else if (type == Mesh::MeshType::spot_light)
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
}