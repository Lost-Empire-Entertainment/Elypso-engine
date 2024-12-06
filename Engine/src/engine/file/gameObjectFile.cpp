//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>

//external
#include "magic_enum.hpp"
#include "glm.hpp"

//engine
#include "gameObjectFile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include "importer.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "directionallight.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "gameobject.hpp"
#include "texture.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lighttcomponent.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using std::ifstream;
using std::ofstream;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using glm::vec3;
using std::filesystem::is_empty;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::make_shared;
using std::cout;
using std::to_string;
using std::unordered_map;
using std::shared_ptr;
using std::vector;

using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::File;
using Utils::String;
using Graphics::Shape::Importer;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;
using Graphics::Shape::DirectionalLight;
using Graphics::Components::Mesh;
using Graphics::Components::Material;
using Graphics::Components::PointLightComponent;
using Graphics::Components::SpotLightComponent;
using Graphics::Components::DirectionalLightComponent;
using Core::Select;
using Graphics::Render;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Texture;
using Graphics::Shader;
using Graphics::Shape::GameObject;
#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace EngineFile
{
	void GameObjectFile::SaveGameObjects()
	{
		for (const auto& obj : GameObjectManager::GetObjects())
		{
			if (obj->GetParent() != nullptr
				&& obj->GetComponent<Mesh>()->GetMeshType() == Mesh::MeshType::billboard)
			{
				string objectTxtFilePath = obj->GetTxtFilePath();
				string objectName = obj->GetName();

				auto material = obj->GetComponent<Material>();
				auto mesh = obj->GetComponent<Mesh>();

				vector<string> data;

				//
				// SAVE SCENE OBJECT DATA INTO VECTOR
				//

				data.push_back("name= " + obj->GetName() + "\n");

				data.push_back("id= " + to_string(obj->GetID()) + "\n");

				data.push_back("enabled= " + to_string(obj->IsEnabled()) + "\n");

				data.push_back("mesh enabled= " + to_string(mesh->IsEnabled()) + "\n");

				string type = string(magic_enum::enum_name(mesh->GetMeshType()));
				data.push_back("type= " + type + "\n");

				//position
				float posX = obj->GetTransform()->GetPosition().x;
				float posY = obj->GetTransform()->GetPosition().y;
				float posZ = obj->GetTransform()->GetPosition().z;
				data.push_back(
					"position= " + to_string(posX) + ", "
					+ to_string(posY) + ", "
					+ to_string(posZ) + "\n");

				//rotation
				float rotX = obj->GetTransform()->GetRotation().x;
				float rotY = obj->GetTransform()->GetRotation().y;
				float rotZ = obj->GetTransform()->GetRotation().z;
				data.push_back(
					"rotation= " + to_string(rotX) + ", "
					+ to_string(rotY) + ", "
					+ to_string(rotZ) + "\n");

				//scale
				float scaleX = obj->GetTransform()->GetScale().x;
				float scaleY = obj->GetTransform()->GetScale().y;
				float scaleZ = obj->GetTransform()->GetScale().z;
				data.push_back(
					"scale= " + to_string(scaleX) + ", "
					+ to_string(scaleY) + ", "
					+ to_string(scaleZ) + "\n");

				data.push_back("\n");

				//object textures
				Mesh::MeshType meshType = mesh->GetMeshType();
				if (meshType == Mesh::MeshType::model)
				{
					string diffuseTexture = material->GetTextureName(Material::TextureType::diffuse);
					diffuseTexture = path(diffuseTexture).filename().string();
					if (diffuseTexture == "diff_default.png") diffuseTexture = "DEFAULTDIFF";

					string specularTexture = material->GetTextureName(Material::TextureType::specular);
					specularTexture = path(specularTexture).filename().string();
					if (specularTexture == "spec_default.png") specularTexture = "DEFAULTSPEC";

					string normalTexture = material->GetTextureName(Material::TextureType::normal);
					normalTexture = path(normalTexture).filename().string();

					string heightTexture = material->GetTextureName(Material::TextureType::height);
					heightTexture = path(heightTexture).filename().string();

					data.push_back(
						+"textures= "
						+ diffuseTexture + ", "
						+ specularTexture + ", "
						+ normalTexture + ", "
						+ heightTexture + "\n");
				}

				//shaders
				string vertexShader = material->GetShaderName(0);
				vertexShader = path(vertexShader).filename().string();
				string fragmentShader = material->GetShaderName(1);
				fragmentShader = path(fragmentShader).filename().string();
				data.push_back("shaders= " + vertexShader + ", " + fragmentShader + "\n");

				//path to txt file of this gameobject
				data.push_back("txtFile= " + obj->GetTxtFilePath() + "\n");

				//material variables
				if (meshType == Mesh::MeshType::model)
				{
					data.push_back("shininess= " + to_string(32) + "\n");
				}
				else if (meshType == Mesh::MeshType::point_light)
				{
					auto pointLightComponent = obj->GetComponent<PointLightComponent>();

					float pointDiffuseX = pointLightComponent->GetDiffuse().x;
					float pointDiffuseY = pointLightComponent->GetDiffuse().y;
					float pointDiffuseZ = pointLightComponent->GetDiffuse().z;
					data.push_back(
						"diffuse= " + to_string(pointDiffuseX) + ", "
						+ to_string(pointDiffuseY) + ", "
						+ to_string(pointDiffuseZ) + "\n");

					data.push_back("intensity= " + to_string(pointLightComponent->GetIntensity()) + "\n");

					data.push_back("distance= " + to_string(pointLightComponent->GetDistance()) + "\n");
				}
				else if (meshType == Mesh::MeshType::spot_light)
				{
					auto spotLightComponent = obj->GetComponent<SpotLightComponent>();

					float spotDiffuseX = spotLightComponent->GetDiffuse().x;
					float spotDiffuseY = spotLightComponent->GetDiffuse().y;
					float spotDiffuseZ = spotLightComponent->GetDiffuse().z;
					data.push_back(
						"diffuse= " + to_string(spotDiffuseX) + ", "
						+ to_string(spotDiffuseY) + ", "
						+ to_string(spotDiffuseZ) + "\n");

					data.push_back("intensity= " + to_string(spotLightComponent->GetIntensity()) + "\n");

					data.push_back("distance= " + to_string(spotLightComponent->GetDistance()) + "\n");

					data.push_back("inner angle= " + to_string(spotLightComponent->GetInnerAngle()) + "\n");

					data.push_back("outer angle= " + to_string(spotLightComponent->GetOuterAngle()) + "\n");
				}
				else if (meshType == Mesh::MeshType::directional_light)
				{
					auto dirLightComponent = obj->GetComponent<DirectionalLightComponent>();

					float dirDiffuseX = dirLightComponent->GetDiffuse().x;
					float dirDiffuseY = dirLightComponent->GetDiffuse().y;
					float dirDiffuseZ = dirLightComponent->GetDiffuse().z;
					data.push_back(
						"diffuse= " + to_string(dirDiffuseX) + ", "
						+ to_string(dirDiffuseY) + ", "
						+ to_string(dirDiffuseZ) + "\n");

					data.push_back("intensity= " + to_string(dirLightComponent->GetIntensity()) + "\n");
				}

				//also save billboard data of each light source
				if (meshType == Mesh::MeshType::point_light
					|| meshType == Mesh::MeshType::spot_light
					|| meshType == Mesh::MeshType::directional_light)
				{
					shared_ptr<GameObject> childBillboard = nullptr;
					for (const auto& child : obj->GetChildren())
					{
						if (child->GetComponent<Mesh>()->GetMeshType() == Mesh::MeshType::billboard)
						{
							childBillboard = child;
							break;
						}
					}

					data.push_back("\n");
					data.push_back("---attached billboard data---\n");
					data.push_back("\n");

					data.push_back("billboard name= " + childBillboard->GetName() + "\n");

					data.push_back("billboard id= " + to_string(childBillboard->GetID()) + "\n");

					data.push_back("billboard enabled= " + to_string(childBillboard->IsEnabled()) + "\n");

					string billboardVertShader = childBillboard->GetComponent<Material>()->GetShaderName(0);
					billboardVertShader = path(billboardVertShader).filename().string();
					string billboardFragShader = childBillboard->GetComponent<Material>()->GetShaderName(1);
					billboardFragShader = path(billboardFragShader).filename().string();
					data.push_back("billboard shaders= " + billboardVertShader + ", " + billboardFragShader + "\n");

					string billboardTexture = childBillboard->GetComponent<Material>()->GetTextureName(Material::TextureType::diffuse);
					billboardTexture = path(billboardTexture).filename().string();
					data.push_back("billboard texture= " + billboardTexture + "\n");

					data.push_back("billboard shininess= " + to_string(32) + "\n");
				}

				//
				// WRITE ALL DATA INTO NEW TXT FILE
				//

				string folderPath = path(objectTxtFilePath).parent_path().string();
				if (!exists(folderPath))
				{
					File::CreateNewFolder(folderPath);
				}

				if (exists(objectTxtFilePath)) File::DeleteFileOrfolder(objectTxtFilePath);

				ofstream objectFile(objectTxtFilePath);

				if (!objectFile.is_open())
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Couldn't write into object txt file path '" + objectTxtFilePath + "'!\n");
					return;
				}

				for (const string dataLine : data)
				{
					objectFile << dataLine;
				}

				objectFile.close();

				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::DEBUG,
					"Successfully saved gameobject " + obj->GetName() + " with ID " + to_string(obj->GetID()) + ".\n");
			}
		}
	}

	string GameObjectFile::GetType(const string& file)
	{
		ifstream objFile(file);

		if (!objFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Couldn't read from object file '" + file + "'!\n");
			return "";
		}

		string target = "type= ";
		string line;
		while (getline(objFile, line))
		{
			size_t pos = line.find(target);
			if (pos != string::npos)
			{
				return line.substr(pos + target.length());
			}
		}
		objFile.close();

		return "";
	}

	void GameObjectFile::LoadGameObjects()
	{
		if (Engine::currentGameobjectsPath.empty())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::DEBUG,
				"There were no gameobjects to load for scene '" + path(Engine::scenePath).parent_path().stem().string() + "'.\n");

			return;
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Started loading gameobjects for scene '" + path(Engine::scenePath).parent_path().stem().string() + "'.\n");
#if ENGINE_MODE
		GUISceneWindow::waitBeforeCountsUpdate = true;
#endif
		vector<string> validAssimpPaths;
		vector<string> validGameobjectPaths;

		//look through parent gameobjects folder
		for (const auto& folder : directory_iterator(Engine::currentGameobjectsPath))
		{
			//look for assimp model files (fbx, glfw, obj)
			for (const auto& file : directory_iterator(folder))
			{
				string extension = path(file).extension().string();
				if (extension == ".fbx"
					|| extension == ".obj"
					|| extension == ".glfw")
				{
					validAssimpPaths.push_back(path(file).string());
				}
			}
			//look through child gameobject folders
			for (const auto& file : directory_iterator(folder))
			{
				//look for light txt files
				if (is_regular_file(file))
				{
					string extension = path(file).extension().string();
					if (extension == ".txt")
					{
						validGameobjectPaths.push_back(path(file).string());
					}
				}
			}
		}

		//loads all assimp models
		for (const auto& assimpPath : validAssimpPaths)
		{
			string fileName = path(assimpPath).stem().string();
			string modelPath = path(assimpPath).string();

			Importer::Initialize(
				vec3(0),
				vec3(0),
				vec3(1),
				modelPath,
				(path(Engine::filesPath) / "shaders" / "GameObject.vert").string(),
				(path(Engine::filesPath) / "shaders" / "GameObject.frag").string(),
				"DEFAULTDIFF",
				"DEFAULTSPEC",
				"EMPTY",
				"EMPTY",
				32,
				fileName,
				Importer::tempID);
		}

		//loads all light txt files
		for (const auto& gameobjectPath : validGameobjectPaths)
		{
			string filePath = path(gameobjectPath).string();

			if (GetType(filePath) == "point_light")
			{
				LoadPointLight(filePath);
			}
			else if (GetType(filePath) == "spot_light")
			{
				LoadSpotlight(filePath);
			}
			else if (GetType(filePath) == "directional_light")
			{
				LoadDirectionalLight(filePath);
			}
		}
#if ENGINE_MODE
		GUISceneWindow::waitBeforeCountsUpdate = false;
		GUISceneWindow::UpdateCounts();
#endif
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Finished loading gameobjects for scene '" + path(Engine::scenePath).parent_path().stem().string() + "'.\n");
	}

	void GameObjectFile::LoadModel(const string& txtFilePath)
	{
		//skip running this function for newly imported models that dont actually
		//have a txt file yet even though their txt file path was assigned
		if (!exists(txtFilePath)) return;

		//
		// READ FROM MODEL FILE
		//

		ifstream txtFile(txtFilePath);
		if (!txtFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Failed to open model txt file '" + txtFilePath + "'!\n\n");
			return;
		}

		unordered_map<string, string> data;
		string line;
		while (getline(txtFile, line))
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
					|| key == "enabled"
					|| key == "mesh enabled"
					|| key == "type"
					|| key == "position"
					|| key == "rotation"
					|| key == "scale"

					|| key == "textures"
					|| key == "shaders"
					|| key == "model"
					|| key == "shininess")
				{
					data[key] = value;
				}
			}
		}

		txtFile.close();

		//
		// ASSIGN MODEL DATA TO VARIABLES
		//

		string name{};
		unsigned int ID{};
		bool isEnabled{};
		bool isMeshEnabled{};
		Mesh::MeshType type{};
		vec3 pos{};
		vec3 rot{};
		vec3 scale{};

		vector<string> textures{};
		vector<string> shaders{};
		string model{};
		float shininess{};

		for (const auto& [key, value] : data)
		{
			if (key == "name")
			{
				name = value;
			}
			else if (key == "id")
			{
				ID = stoul(value);
			}
			else if (key == "enabled")
			{
				isEnabled = stoi(value);
			}
			else if (key == "mesh enabled")
			{
				isMeshEnabled = stoi(value);
			}
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

			else if (key == "textures")
			{
				vector<string> split = String::Split(value, ',');

				string fullTex0Path = split[0] == "DEFAULTDIFF"
					? "DEFAULTDIFF"
					: (path(txtFilePath).parent_path() / split[0]).string();

				if (fullTex0Path == "DEFAULTDIFF")
				{
					fullTex0Path = (path(Engine::filesPath) / "textures" / "diff_default.png").string();
					textures.push_back(fullTex0Path);
				}
				else if (fullTex0Path != "DEFAULTDIFF"
						 && !exists(fullTex0Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Texture at slot 0 for " + name + " at " + fullTex0Path + " does not exist!\n");
					textures.push_back((path(Engine::filesPath) / "textures" / "diff_missing.png").string());
				}
				else textures.push_back(fullTex0Path);

				string fullTex1Path = split[1] == "DEFAULTSPEC"
					? "DEFAULTSPEC"
					: (path(txtFilePath).parent_path() / split[1]).string();
				if (fullTex1Path == "DEFAULTSPEC")
				{
					fullTex1Path = (path(Engine::filesPath) / "textures" / "spec_default.png").string();
					textures.push_back(fullTex1Path);
				}
				else if (fullTex1Path != "DEFAULTSPEC"
					&& !exists(fullTex1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Texture at slot 1 for " + name + " at " + fullTex1Path + " does not exist!\n");
					textures.push_back("DEFAULTSPEC");
				}
				else textures.push_back(fullTex1Path);

				string fullTex2Path = split[2] == "EMPTY"
					? "EMPTY"
					: (path(txtFilePath).parent_path() / split[2]).string();
				if (fullTex2Path != "EMPTY"
					&& !exists(fullTex2Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Texture at slot 2 for " + name + " at " + fullTex2Path + " does not exist!\n");
					textures.push_back("EMPTY");
				}
				else textures.push_back(fullTex2Path);

				string fullTex3Path = split[3] == "EMPTY"
					? "EMPTY"
					: (path(txtFilePath).parent_path() / split[3]).string();
				if (fullTex3Path != "EMPTY"
					&& !exists(fullTex3Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Texture at slot 3 for " + name + " at " + fullTex3Path + " does not exist!\n");
					textures.push_back("EMPTY");
				}
				else textures.push_back(fullTex3Path);
			}
			else if (key == "shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = (path(Engine::filesPath) / "shaders" / split[0]).string();
				string fullShader1Path = (path(Engine::filesPath) / "shaders" / split[1]).string();

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: One or more shaders are missing for " + name + " at " + value + "! Skipped loading gameobject.\n");
					return;
				}
				else
				{
					shaders.push_back(fullShader0Path);
					shaders.push_back(fullShader1Path);
				}
			}
			else if (key == "model")
			{
				model = value;
			}
			else if (key == "shininess")
			{
				shininess = stof(value);
			}
		}

		//
		// SET UP TEXTURES FOR MODEL
		//

		string diff_missing = (path(Engine::filesPath) / "textures" / "diff_missing.png").string();
		string diffuseTexture = textures[0];
		if (diffuseTexture != "EMPTY"
			&& !exists(diffuseTexture))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Diffuse texture " + diffuseTexture + " for " + name + " not found!\n");
			diffuseTexture = diff_missing;
		}

		string specularTexture = textures[1];
		if (specularTexture != "EMPTY"
			&& !exists(specularTexture))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Specular texture " + specularTexture + " for " + name + " not found!\n");
			specularTexture = "EMPTY";
		}

		string normalTexture = textures[2];
		if (normalTexture != "EMPTY"
			&& !exists(normalTexture))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Normal texture " + normalTexture + " for " + name + " not found!\n");
			normalTexture = "EMPTY";
		}

		string heightTexture = textures[3];
		if (heightTexture != "EMPTY"
			&& !exists(heightTexture))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Height texture " + heightTexture + " for " + name + " not found!\n");
			heightTexture = "EMPTY";
		}

		//
		// LOAD MODEL
		//
 
		shared_ptr<GameObject> foundObj;
		for (const auto& obj : GameObjectManager::GetObjects())
		{
			if (obj->GetName() == name)
			{
				foundObj = obj;
				break;
			}
		}

		if (foundObj == nullptr)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Tried to apply data to scene model '" + name + "' with ID '" + to_string(ID) + "' but it does not exist!\n");
		}
		else
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::DEBUG,
				"Loading model '" + name + "' with ID '" + to_string(ID) + "' for scene '" + path(Engine::scenePath).parent_path().stem().string() + "'.\n");

			foundObj->SetName(name);
			foundObj->SetID(ID);
			foundObj->SetEnableState(isEnabled);

			foundObj->GetTransform()->SetPosition(pos);
			foundObj->GetTransform()->SetRotation(rot);
			foundObj->GetTransform()->SetScale(scale);

			Shader modelShader = Shader::LoadShader(shaders[0], shaders[1]);
			shared_ptr<Material> mat = make_shared<Material>();
			mat->AddShader(shaders[0], shaders[1], modelShader);

			Texture::LoadTexture(foundObj, diffuseTexture, Material::TextureType::diffuse, false);
			Texture::LoadTexture(foundObj, specularTexture, Material::TextureType::specular, false);
			Texture::LoadTexture(foundObj, normalTexture, Material::TextureType::height, false);
			Texture::LoadTexture(foundObj, heightTexture, Material::TextureType::normal, false);

			/*
			* 
			* SHININESS VARIABLE IS CURRENTLY UNUSED
			* IT WILL BE RE-ENABLED IN THE FUTURE
			* 
			foundObj->GetBasicShape()->SetShininess(shininess);
			*/

			GameObject::nextID = ID + 1;
		}
	}

	void GameObjectFile::LoadPointLight(const string& file)
	{
		//
		// READ FROM POINT LIGHT FILE
		//

		ifstream pointLightFile(file);
		if (!pointLightFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Failed to open point light file '" + file + "'!\n\n");
			return;
		}

		unordered_map<string, string> data;
		string line;
		while (getline(pointLightFile, line))
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
					|| key == "enabled"
					|| key == "mesh enabled"
					|| key == "type"
					|| key == "position"
					|| key == "rotation"
					|| key == "scale"

					|| key == "shaders"
					|| key == "txtFile"

					|| key == "diffuse"
					|| key == "intensity"
					|| key == "distance"

					|| key == "billboard name"
					|| key == "billboard id"
					|| key == "billboard enabled"
					|| key == "billboard shaders"
					|| key == "billboard texture"
					|| key == "billboard shininess")
				{
					data[key] = value;
				}
			}
		}

		pointLightFile.close();

		//
		// ASSIGN POINT LIGHT DATA TO VARIABLES
		//

		string name{};
		unsigned int ID{};
		bool isEnabled{};
		bool isMeshEnabled{};
		Mesh::MeshType type{};
		vec3 pos{};
		vec3 rot{};
		vec3 scale{};

		vector<string> shaders{};
		string txtFile{};

		vec3 diffuse{};
		float intensity{};
		float distance{};

		string billboardName{};
		unsigned int billboardID{};
		bool isBillboardEnabled{};
		vector<string> billboardShaders{};
		string billboardTexture{};
		float billboardShininess{};

		for (const auto& [key, value] : data)
		{
			if (key == "name")
			{
				name = value;
			}
			else if (key == "id")
			{
				ID = stoul(value);
			}
			else if (key == "enabled")
			{
				isEnabled = stoi(value);
			}
			else if (key == "mesh enabled")
			{
				isMeshEnabled = stoi(value);
			}
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

			else if (key == "shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = (path(Engine::filesPath) / "shaders" / split[0]).string();
				string fullShader1Path = (path(Engine::filesPath) / "shaders" / split[1]).string();

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: One or more shaders are missing for " + name + " at " + value + "! Skipped loading gameobject.\n");
					return;
				}
				else
				{
					shaders.push_back(fullShader0Path);
					shaders.push_back(fullShader1Path);
				}
			}
			else if (key == "txtFile")
			{
				txtFile = value;
			}

			else if (key == "diffuse")
			{
				vector<string> split = String::Split(value, ',');
				vec3 newDiffuse = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
				diffuse = newDiffuse;
			}
			else if (key == "intensity") intensity = stof(value);
			else if (key == "distance") distance = stof(value);

			else if (key == "billboard name")
			{
				billboardName = value;
			}
			else if (key == "billboard id")
			{
				billboardID = stoul(value);
			}
			else if (key == "billboard enabled")
			{
				isBillboardEnabled = stoi(value);
			}
			else if (key == "billboard texture")
			{
				string fullTexPath = (path(Engine::filesPath) / "icons" / value).string();

				if (!exists(fullTexPath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Texture is missing for " + name + " at " + fullTexPath + "! Skipped loading billboard.\n");
					return;
				}
				else billboardTexture = fullTexPath;
			}
			else if (key == "billboard shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = (path(Engine::filesPath) / "shaders" / split[0]).string();
				string fullShader1Path = (path(Engine::filesPath) / "shaders" / split[1]).string();

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: One or more shaders are missing for " + name + " at " + value + "! Skipped loading billboard.\n");
					return;
				}
				else
				{
					billboardShaders.push_back(fullShader0Path);
					billboardShaders.push_back(fullShader1Path);
				}
			}
			else if (key == "billboard shininess") billboardShininess = stof(value);
		}

		//
		// LOAD POINT LIGHT
		//

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Loading point light '" + name + "' with ID '" + to_string(ID) + "' for scene '" + path(Engine::scenePath).parent_path().stem().string() + "'.\n");

		PointLight::InitializePointLight(
			pos,
			rot,
			scale,
			txtFile,
			shaders[0],
			shaders[1],
			diffuse,
			intensity,
			distance,
			name,
			ID,
			isEnabled,
			isMeshEnabled,
			billboardShaders[0],
			billboardShaders[1],
			billboardTexture,
			billboardShininess,
			billboardName,
			billboardID,
			isBillboardEnabled);

		GameObject::nextID = ID + 1;
	}

	void GameObjectFile::LoadSpotlight(const string& file)
	{
		//
		// READ FROM SPOTLIGHT FILE
		//

		ifstream spotLightFile(file);
		if (!spotLightFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Failed to open spotlight file '" + file + "'!\n\n");
			return;
		}

		unordered_map<string, string> data;
		string line;
		while (getline(spotLightFile, line))
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
					|| key == "enabled"
					|| key == "mesh enabled"
					|| key == "type"
					|| key == "position"
					|| key == "rotation"
					|| key == "scale"

					|| key == "shaders"
					|| key == "txtFile"

					|| key == "diffuse"
					|| key == "intensity"
					|| key == "distance"
					|| key == "inner angle"
					|| key == "outer angle"

					|| key == "billboard name"
					|| key == "billboard id"
					|| key == "billboard enabled"
					|| key == "billboard shaders"
					|| key == "billboard texture"
					|| key == "billboard shininess")
				{
					data[key] = value;
				}
			}
		}

		spotLightFile.close();

		//
		// ASSIGN SPOTLIGHT DATA TO VARIABLES
		//

		string name{};
		unsigned int ID{};
		bool isEnabled{};
		bool isMeshEnabled{};
		Mesh::MeshType type{};
		vec3 pos{};
		vec3 rot{};
		vec3 scale{};

		vector<string> shaders{};
		string txtFile{};

		vec3 diffuse{};
		float intensity{};
		float distance{};
		float innerAngle{};
		float outerAngle{};

		string billboardName{};
		unsigned int billboardID{};
		bool isBillboardEnabled{};
		vector<string> billboardShaders{};
		string billboardTexture{};
		float billboardShininess{};

		for (const auto& [key, value] : data)
		{
			if (key == "name")
			{
				name = value;
			}
			else if (key == "id")
			{
				ID = stoul(value);
			}
			else if (key == "enabled")
			{
				isEnabled = stoi(value);
			}
			else if (key == "mesh enabled")
			{
				isMeshEnabled = stoi(value);
			}
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

			else if (key == "shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = (path(Engine::filesPath) / "shaders" / split[0]).string();
				string fullShader1Path = (path(Engine::filesPath) / "shaders" / split[1]).string();

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: One or more shaders are missing for " + name + " at " + value + "! Skipped loading gameobject.\n");
					return;
				}
				else
				{
					shaders.push_back(fullShader0Path);
					shaders.push_back(fullShader1Path);
				}
			}
			else if (key == "txtFile")
			{
				txtFile = value;
			}

			else if (key == "diffuse")
			{
				vector<string> split = String::Split(value, ',');
				vec3 newDiffuse = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
				diffuse = newDiffuse;
			}
			else if (key == "intensity") intensity = stof(value);
			else if (key == "distance") distance = stof(value);
			else if (key == "inner angle") innerAngle = stof(value);
			else if (key == "outer angle") outerAngle = stof(value);

			else if (key == "billboard name")
			{
				billboardName = value;
			}
			else if (key == "billboard id")
			{
				billboardID = stoul(value);
			}
			else if (key == "billboard enabled")
			{
				isBillboardEnabled = stoi(value);
			}
			else if (key == "billboard texture")
			{
				string fullTexPath = (path(Engine::filesPath) / "icons" / value).string();

				if (!exists(fullTexPath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Texture is missing for " + name + " at " + fullTexPath + "! Skipped loading billboard.\n");
					return;
				}
				else billboardTexture = fullTexPath;
			}
			else if (key == "billboard shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = (path(Engine::filesPath) / "shaders" / split[0]).string();
				string fullShader1Path = (path(Engine::filesPath) / "shaders" / split[1]).string();

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: One or more shaders are missing for " + name + " at " + value + "! Skipped loading billboard.\n");
					return;
				}
				else
				{
					billboardShaders.push_back(fullShader0Path);
					billboardShaders.push_back(fullShader1Path);
				}
			}
			else if (key == "billboard shininess") billboardShininess = stof(value);
		}

		//
		// LOAD SPOTLIGHT
		//

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Loading spotlight '" + name + "' with ID '" + to_string(ID) + "' for scene '" + path(Engine::scenePath).parent_path().stem().string() + "'.\n");

		SpotLight::InitializeSpotLight(
			pos,
			rot,
			scale,
			txtFile,
			shaders[0],
			shaders[1],
			diffuse,
			intensity,
			distance,
			innerAngle,
			outerAngle,
			name,
			ID,
			isEnabled,
			isMeshEnabled,
			billboardShaders[0],
			billboardShaders[1],
			billboardTexture,
			billboardShininess,
			billboardName,
			billboardID,
			isBillboardEnabled);

		GameObject::nextID = ID + 1;
	}

	void GameObjectFile::LoadDirectionalLight(const string& file)
	{
		//
		// READ FROM DIRECTIONAL LIGHT FILE
		//

		ifstream directionalLightFile(file);
		if (!directionalLightFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Failed to open directional light file '" + file + "'!\n\n");
			return;
		}

		unordered_map<string, string> data;
		string line;
		while (getline(directionalLightFile, line))
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
					|| key == "enabled"
					|| key == "mesh enabled"
					|| key == "type"
					|| key == "position"
					|| key == "rotation"
					|| key == "scale"

					|| key == "shaders"
					|| key == "txtFile"

					|| key == "diffuse"
					|| key == "intensity"
					|| key == "distance"

					|| key == "billboard name"
					|| key == "billboard id"
					|| key == "billboard enabled"
					|| key == "billboard shaders"
					|| key == "billboard texture"
					|| key == "billboard shininess")
				{
					data[key] = value;
				}
			}
		}

		directionalLightFile.close();

		//
		// ASSIGN DIRECTIONAL LIGHT DATA TO VARIABLES
		//

		string name{};
		unsigned int ID{};
		bool isEnabled{};
		bool isMeshEnabled{};
		Mesh::MeshType type{};
		vec3 pos{};
		vec3 rot{};
		vec3 scale{};

		vector<string> shaders{};
		string txtFile{};

		vec3 diffuse{};
		float intensity{};

		string billboardName{};
		unsigned int billboardID{};
		bool isBillboardEnabled{};
		vector<string> billboardShaders{};
		string billboardTexture{};
		float billboardShininess{};

		for (const auto& [key, value] : data)
		{
			if (key == "name")
			{
				name = value;
			}
			else if (key == "id")
			{
				ID = stoul(value);
			}
			else if (key == "enabled")
			{
				isEnabled = stoi(value);
			}
			else if (key == "mesh enabled")
			{
				isMeshEnabled = stoi(value);
			}
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

			else if (key == "shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = (path(Engine::filesPath) / "shaders" / split[0]).string();
				string fullShader1Path = (path(Engine::filesPath) / "shaders" / split[1]).string();

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: One or more shaders are missing for " + name + " at " + value + "! Skipped loading gameobject.\n");
					return;
				}
				else
				{
					shaders.push_back(fullShader0Path);
					shaders.push_back(fullShader1Path);
				}
			}
			else if (key == "txtFile")
			{
				txtFile = value;
			}

			else if (key == "diffuse")
			{
				vector<string> split = String::Split(value, ',');
				vec3 newDiffuse = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
				diffuse = newDiffuse;
			}
			else if (key == "intensity") intensity = stof(value);

			else if (key == "billboard name")
			{
				billboardName = value;
			}
			else if (key == "billboard id")
			{
				billboardID = stoul(value);
			}
			else if (key == "billboard enabled")
			{
				isBillboardEnabled = stoi(value);
			}
			else if (key == "billboard texture")
			{
				string fullTexPath = (path(Engine::filesPath) / "icons" / value).string();

				if (!exists(fullTexPath))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: Texture is missing for " + name + " at " + fullTexPath + "! Skipped loading billboard.\n");
					return;
				}
				else billboardTexture = fullTexPath;
			}
			else if (key == "billboard shaders")
			{
				vector<string> split = String::Split(value, ',');

				string fullShader0Path = (path(Engine::filesPath) / "shaders" / split[0]).string();
				string fullShader1Path = (path(Engine::filesPath) / "shaders" / split[1]).string();

				if (!exists(fullShader0Path)
					|| !exists(fullShader1Path))
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::FILE,
						Type::EXCEPTION,
						"Error: One or more shaders are missing for " + name + " at " + value + "! Skipped loading billboard.\n");
					return;
				}
				else
				{
					billboardShaders.push_back(fullShader0Path);
					billboardShaders.push_back(fullShader1Path);
				}
			}
			else if (key == "billboard shininess") billboardShininess = stof(value);
		}

		//
		// LOAD DIRECTIONAL LIGHT
		//

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Loading directional light '" + name + "' with ID '" + to_string(ID) + "' for scene '" + path(Engine::scenePath).parent_path().stem().string() + "'.\n");

		DirectionalLight::InitializeDirectionalLight(
			pos,
			rot,
			scale,
			txtFile,
			shaders[0],
			shaders[1],
			diffuse,
			intensity,
			name,
			ID,
			isEnabled,
			isMeshEnabled,
			billboardShaders[0],
			billboardShaders[1],
			billboardTexture,
			billboardShininess,
			billboardName,
			billboardID,
			isBillboardEnabled);

		GameObject::nextID = ID + 1;
	}
}