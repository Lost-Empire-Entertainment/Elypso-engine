//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>

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

using std::ifstream;
using std::ofstream;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using glm::vec3;
using std::filesystem::is_empty;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::vector;
using std::make_shared;
using std::cout;
using std::to_string;

using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::File;
using Graphics::Shape::Mesh;
using Utils::String;
using Graphics::Shape::Material;
using Graphics::Shape::Importer;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;
using Graphics::Shape::DirectionalLight;
using Core::Select;
using Graphics::Render;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Texture;
using Graphics::Shader;

namespace EngineFile
{
	void GameObjectFile::SaveGameObjects()
	{
		for (const auto& obj : GameObjectManager::GetObjects())
		{
			if (obj->GetParentBillboardHolder() == nullptr)
			{
				string objectTxtFilePath = obj->GetTxtFilePath();
				string objectName = obj->GetName();

				vector<string> data;

				//
				// GET IMPORTED MODEL DATA FROM FILE
				//

				if (objectTxtFilePath != ""
					&& exists(objectTxtFilePath))
				{
					ifstream existingData(objectTxtFilePath);
					if (!existingData.is_open())
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::FILE,
							Type::EXCEPTION,
							"Error: Couldn't read from object txt file path '" + objectTxtFilePath + "'!\n");
						return;
					}
				}

				//
				// SAVE SCENE OBJECT DATA INTO VECTOR
				//

				data.push_back("name= " + obj->GetName() + "\n");

				data.push_back("id= " + to_string(obj->GetID()) + "\n");

				data.push_back("enabled= " + to_string(obj->IsEnabled()) + "\n");

				data.push_back("mesh enabled= " + to_string(obj->GetMesh()->IsEnabled()) + "\n");

				string type = string(magic_enum::enum_name(obj->GetMesh()->GetMeshType()));
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

					data.push_back(
						+"textures= "
						+ diffuseTexture + ", "
						+ specularTexture + ", "
						+ normalTexture + ", "
						+ heightTexture + "\n");
				}

				//shaders
				string vertexShader = obj->GetMaterial()->GetShaderName(0);
				vertexShader = path(vertexShader).filename().string();
				string fragmentShader = obj->GetMaterial()->GetShaderName(1);
				fragmentShader = path(fragmentShader).filename().string();
				data.push_back("shaders= " + vertexShader + ", " + fragmentShader + "\n");

				//path to txt file of this gameobject
				string txtFilePath = String::CharReplace(obj->GetTxtFilePath(), '/', '\\');
				data.push_back("txtFile= " + txtFilePath + "\n");

				//path to model of this gameobject
				string modelPath = String::CharReplace(obj->GetModelPath(), '/', '\\');
				data.push_back("model= " + modelPath + "\n");

				//material variables
				if (meshType == Mesh::MeshType::model)
				{
					data.push_back("shininess= " + to_string(obj->GetBasicShape()->GetShininess()) + "\n");
				}
				else if (meshType == Mesh::MeshType::point_light)
				{
					float pointDiffuseX = obj->GetPointLight()->GetDiffuse().x;
					float pointDiffuseY = obj->GetPointLight()->GetDiffuse().y;
					float pointDiffuseZ = obj->GetPointLight()->GetDiffuse().z;
					data.push_back(
						"diffuse= " + to_string(pointDiffuseX) + ", "
						+ to_string(pointDiffuseY) + ", "
						+ to_string(pointDiffuseZ) + "\n");

					data.push_back("intensity= " + to_string(obj->GetPointLight()->GetIntensity()) + "\n");

					data.push_back("distance= " + to_string(obj->GetPointLight()->GetDistance()) + "\n");
				}
				else if (meshType == Mesh::MeshType::spot_light)
				{
					float spotDiffuseX = obj->GetSpotLight()->GetDiffuse().x;
					float spotDiffuseY = obj->GetSpotLight()->GetDiffuse().y;
					float spotDiffuseZ = obj->GetSpotLight()->GetDiffuse().z;
					data.push_back(
						"diffuse= " + to_string(spotDiffuseX) + ", "
						+ to_string(spotDiffuseY) + ", "
						+ to_string(spotDiffuseZ) + "\n");

					data.push_back("intensity= " + to_string(obj->GetSpotLight()->GetIntensity()) + "\n");

					data.push_back("distance= " + to_string(obj->GetSpotLight()->GetDistance()) + "\n");

					data.push_back("inner angle= " + to_string(obj->GetSpotLight()->GetInnerAngle()) + "\n");

					data.push_back("outer angle= " + to_string(obj->GetSpotLight()->GetOuterAngle()) + "\n");
				}
				else if (meshType == Mesh::MeshType::directional_light)
				{
					float dirDiffuseX = obj->GetDirectionalLight()->GetDiffuse().x;
					float dirDiffuseY = obj->GetDirectionalLight()->GetDiffuse().y;
					float dirDiffuseZ = obj->GetDirectionalLight()->GetDiffuse().z;
					data.push_back(
						"diffuse= " + to_string(dirDiffuseX) + ", "
						+ to_string(dirDiffuseY) + ", "
						+ to_string(dirDiffuseZ) + "\n");

					data.push_back("intensity= " + to_string(obj->GetDirectionalLight()->GetIntensity()) + "\n");
				}

				//also save billboard data of each light source
				if (meshType == Mesh::MeshType::point_light
					|| meshType == Mesh::MeshType::spot_light
					|| meshType == Mesh::MeshType::directional_light)
				{
					data.push_back("\n");
					data.push_back("---attatched billboard data---\n");
					data.push_back("\n");

					data.push_back("billboard name= " + obj->GetChildBillboard()->GetName() + "\n");

					data.push_back("billboard id= " + to_string(obj->GetChildBillboard()->GetID()) + "\n");

					data.push_back("billboard enabled= " + to_string(obj->GetChildBillboard()->IsEnabled()) + "\n");

					string billboardVertShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(0);
					billboardVertShader = path(billboardVertShader).filename().string();
					string billboardFragShader = obj->GetChildBillboard()->GetMaterial()->GetShaderName(1);
					billboardFragShader = path(billboardFragShader).filename().string();
					data.push_back("billboard shaders= " + billboardVertShader + ", " + billboardFragShader + "\n");

					string billboardTexture = obj->GetChildBillboard()->GetMaterial()->GetTextureName(Material::TextureType::diffuse);
					billboardTexture = path(billboardTexture).filename().string();
					data.push_back("billboard texture= " + billboardTexture + "\n");

					data.push_back("billboard shininess= " + to_string(obj->GetChildBillboard()->GetBasicShape()->GetShininess()) + "\n");
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
		if (!Engine::currentGameobjectsPath.empty())
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
		Render::waitBeforeCountsUpdate = true;
#endif
		vector<string> validAssimpPaths;
		vector<string> validGameobjectPaths;

		//look through parent gameobjects folder
		for (const auto& folder : directory_iterator(Engine::currentGameobjectsPath))
		{
			//look for assimp model files (fbs, glfw, obj)
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
				//model txt files are always inside their own child gameobject folder
				//so that textures for that specific assimp node file can be separated
				//from the rest of the assimp node txt files for this assimp file
				if (is_directory(file))
				{
					//look for model txt files
					for (const auto& dirFile : directory_iterator(file))
					{
						string extension = path(dirFile).extension().string();
						if (extension == ".txt")
						{
							validGameobjectPaths.push_back(path(dirFile).string());
						}
					}
				}
				//look for light txt files
				else if (is_regular_file(file))
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
			string filePath = path(assimpPath).string();

			Importer::Initialize(
				vec3(0),
				vec3(0),
				vec3(1),
				filePath,
				"",
				Engine::filesPath + "\\shaders\\GameObject.vert",
				Engine::filesPath + "\\shaders\\GameObject.frag",
				"DEFAULTDIFF",
				"DEFAULTSPEC",
				"EMPTY",
				"EMPTY",
				32,
				fileName,
				Importer::tempID);
		}

		//loads all model and light txt files
		for (const auto& gameobjectPath : validGameobjectPaths)
		{
			string filePath = path(gameobjectPath).string();

			if (GetType(filePath) == "model")
			{
				LoadModel(filePath);
			}
			else if (GetType(filePath) == "point_light")
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
		Render::waitBeforeCountsUpdate = false;
		Render::UpdateCounts();
#endif
		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Finished loading gameobjects for scene '" + path(Engine::scenePath).parent_path().stem().string() + "'.\n");
	}

	void GameObjectFile::LoadModel(const string& file)
	{
		//
		// READ FROM MODEL FILE
		//

		ifstream modelFile(file);
		if (!modelFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Failed to open model file '" + file + "'!\n\n");
			return;
		}

		map<string, string> data;
		string line;
		while (getline(modelFile, line))
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
					|| key == "txtFile"
					|| key == "model"
					|| key == "shininess")
				{
					data[key] = value;
				}
			}
		}

		modelFile.close();

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
		string txtFile{};
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
					: path(model).parent_path().string() + "\\" + split[0];

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
						"Error: Texture at slot 0 for " + name + " at " + fullTex0Path + " does not exist!\n");
					textures.push_back(Engine::filesPath + "\\textures\\diff_missing.png");
				}
				else textures.push_back(fullTex0Path);

				string fullTex1Path = split[1] == "DEFAULTSPEC"
					? "DEFAULTSPEC"
					: path(model).parent_path().string() + "\\" + split[1];
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
						"Error: Texture at slot 1 for " + name + " at " + fullTex1Path + " does not exist!\n");
					textures.push_back("DEFAULTSPEC");
				}
				else textures.push_back(fullTex1Path);

				string fullTex2Path = split[2] == "EMPTY"
					? "EMPTY"
					: path(model).parent_path().string() + "\\" + split[2];
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
					: path(model).parent_path().string() + "\\" + split[3];
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

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

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

		string diff_missing = Engine::filesPath + "\\textures\\diff_missing.png";
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

		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
		shared_ptr<GameObject> foundObj;
		for (const auto& obj : objects)
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
				"Error: Tried to apply data to scene model '" + name + "' with ID '" + to_string(ID) + "' but it does not exist!");
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

			foundObj->SetTxtFilePath(txtFile);
			foundObj->SetModelPath(model);

			foundObj->GetBasicShape()->SetShininess(shininess);

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

		map<string, string> data;
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

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

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
				string fullTexPath = Engine::filesPath + "\\icons\\" + value;

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

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

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

		map<string, string> data;
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

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

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
				string fullTexPath = Engine::filesPath + "\\icons\\" + value;

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

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

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

		map<string, string> data;
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

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

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
				string fullTexPath = Engine::filesPath + "\\icons\\" + value;

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

				string fullShader0Path = Engine::filesPath + "\\shaders\\" + split[0];
				string fullShader1Path = Engine::filesPath + "\\shaders\\" + split[1];

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