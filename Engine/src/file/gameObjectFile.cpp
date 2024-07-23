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
		if (!exists(gameobjectsFolder)) File::CreateNewFolder(gameobjectsFolder);

		string objectPath = gameobjectsFolder + "\\" + obj->GetName();
		string objectName = path(objectPath).stem().string();

		if (exists(objectPath))
		{
			File::DeleteFileOrfolder(objectPath + "\\settings.txt");
		}
		else File::CreateNewFolder(objectPath);

		string objectFilePath = objectPath + "\\settings.txt";
		ofstream objectFile(objectFilePath);

		if (!objectFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't write into object file '" + objectFilePath + "'!\n");
			return;
		}

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
			diffuseTexture = String::CharReplace(diffuseTexture, '/', '\\');
			string specularTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::specular);
			specularTexture = String::CharReplace(specularTexture, '/', '\\');
			string normalTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::normal);
			normalTexture = String::CharReplace(normalTexture, '/', '\\');
			string heightTexture = obj->GetMaterial()->GetTextureName(Material::TextureType::height);
			heightTexture = String::CharReplace(heightTexture, '/', '\\');
			objectFile
				<< "textures= "
				<< diffuseTexture << ", "
				<< specularTexture << ", "
				<< normalTexture << ", "
				<< heightTexture << "\n";
		}

		//shaders
		string vertexShader = obj->GetMaterial()->GetShaderName(0);
		vertexShader = String::CharReplace(vertexShader, '/', '\\');
		string fragmentShader = obj->GetMaterial()->GetShaderName(1);
		fragmentShader = String::CharReplace(fragmentShader, '/', '\\');
		objectFile << "shaders= " << vertexShader << ", " << fragmentShader << "\n";

		//material variables
		if (meshType == Mesh::MeshType::model)
		{
			objectFile << "shininess= " << obj->GetBasicShape()->GetShininess() << "\n";

			string modelPath = String::CharReplace(obj->GetDirectory(), '/', '\\');
			objectFile << "model path= " << modelPath << "\n";
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
			string billboardFragShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(1);
			objectFile << "billboard shaders= " << billboardVertShader << ", " << billboardFragShader << "\n";

			objectFile << "billboard texture= " << obj->GetChildBillboard()->GetMaterial()->GetTextureName(Material::TextureType::diffuse) << "\n";

			objectFile << "billboard shininess= " << obj->GetChildBillboard()->GetBasicShape()->GetShininess() << "\n";
		}

		objectFile.close();
	}

	void GameObjectFile::LoadGameObjects(const string& targetPath)
	{
		for (const auto& folder : directory_iterator(targetPath))
		{
			string settingsFile = targetPath + "\\" + path(folder).stem().string() + "\\settings.txt";
		}
	}

	void GameObjectFile::LoadGameObject(const map<string, string>& data, Mesh::MeshType meshType)
	{
		string name;
		unsigned int id;
		vec3 pos;
		vec3 rot;
		vec3 scale;
		string modelPath;
		vector<string> textures;
		vector<string> shaders;

		string billboardName;
		unsigned int billboardID;
		string billboardDiffTexture;
		vector<string> billboardShaders;
		float billboardShininess;

		vec3 diffuse;
		float shininess;
		float intensity;
		float distance;
		float innerAngle;
		float outerAngle;

		//
		// LOAD REGULAR GAMEOBJECT
		//

		if (meshType == Mesh::MeshType::model)
		{
			string diff_missing = Engine::filesPath + "\\textures\\diff_missing.png";
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

		//
		// LOAD POINT LIGHT
		//

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

		//
		// LOAD SPOTLIGHT
		//

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
}