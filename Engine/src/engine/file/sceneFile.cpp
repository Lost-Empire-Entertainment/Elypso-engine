//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

//external
#include "magic_enum.hpp"
#include "glm.hpp"

//engine
#include "sceneFile.hpp"
#include "core.hpp"
#include "gameobject.hpp"
#include "selectobject.hpp"
#include "render.hpp"
#include "model.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "console.hpp"
#include "gameObjectFile.hpp"
#include "skybox.hpp"
#include "physics.hpp"
#include "transformcomponent.hpp"
#include "stringutils.hpp"
#include "fileutils.hpp"

using std::ifstream;
using std::ofstream;
using std::cout;
using std::string;
using std::to_string;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;
using std::vector;
using glm::vec3;
using std::shared_ptr;

using Core::Engine;
using Core::Select;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Render;
using Graphics::Shape::Model;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using EngineFile::GameObjectFile;
using Graphics::Shape::Skybox;
using Core::Physics;
using Graphics::Components::TransformComponent;
using Utils::File;
using Utils::String;

namespace EngineFile
{
	void SceneFile::LoadScene(const string& scenePath)
	{
		if (!exists(scenePath))
		{
			if (scenePath != (path(Engine::scenesPath) / "Scene1" / "scene.txt").string())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: Tried to load scene file '" + scenePath + "' but it doesn't exist! Loading default scene.\n");
			}

			//create new default scene folder if it doesnt exist
			string defaultSceneFolder = (path(Engine::scenesPath) / "Scene1").string();
			if (!exists(defaultSceneFolder))
			{
				File::CreateNewFolder(defaultSceneFolder);
				File::CreateNewFolder((path(defaultSceneFolder) / "gameobjects").string());
			}

			//create new default scene file if it doesnt exist
			string defaultScenePath = (path(defaultSceneFolder) / "scene.txt").string();
			if (!exists(defaultScenePath)) CreateNewSceneFile(defaultScenePath);
		}
		else 
		{
			Engine::scenePath = scenePath;
			Engine::currentGameobjectsPath = (path(Engine::scenePath).parent_path() / "gameobjects").string();

			vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
			if (objects.size() != 0)
			{
				for (const auto& obj : objects)
				{
					GameObjectManager::DestroyGameObject(obj, false);
				}
			}

			ifstream sceneFile(Engine::scenePath);
			if (!sceneFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: Failed to open scene file '" + Engine::scenePath + "'!\n\n");
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

					if (GetKey(key) == "")
					{
						keys.push_back(key);
						values.push_back(value);
					}
					else SetValue(key, value);
				}
			}

			sceneFile.close();

			GameObjectFile::LoadGameObjects();

			Render::SetWindowNameAsUnsaved(false);

			//update project file originating from hub 
			//to ensure currently opened scene is always opened when hub opens engine
			string projectFilePath = (path(Engine::docsPath) / "project.txt").string();
			File::DeleteTarget(projectFilePath);
			ofstream projFile(projectFilePath);

			projFile << "scene: " << Engine::scenePath << "\n";
			projFile << "project: " << Engine::projectPath;

			projFile.close();

			if (Engine::gameFirstScene == "")
			{
				Engine::gameFirstScene = path(scenePath).parent_path().stem().string();

				ConsoleManager::WriteConsoleMessage(
					Caller::INPUT,
					Type::INFO,
					"Set game first scene to " + Engine::gameFirstScene + "\n");
			}
			
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::INFO,
				"Successfully loaded scene file '" + path(Engine::scenePath).parent_path().stem().string() + "'!\n");
		}

		Select::selectedObj = nullptr;
		Select::isObjectSelected = false;
	}

	void SceneFile::SaveScene(SaveType saveType, const string& targetLevel, bool sendSuccessMessage)
	{
		if (exists(sceneFilePath))
		{
			File::DeleteTarget(sceneFilePath);
		}

		GameObjectFile::SaveGameObjects();

		SaveGlobalGraphicsData();
		SaveGlobalPhysicsData();

		ofstream sceneFile(Engine::scenePath);
		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Couldn't write into scene file '" + Engine::scenePath + "'!\n");
			return;
		}

		for (int i = 0; i < keys.size(); i++)
		{
			string key = keys[i];
			string value = values[i];

			sceneFile << key << "= " << value << "\n";
		}

		sceneFile.close();

		Render::SetWindowNameAsUnsaved(false);

		if (sendSuccessMessage)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::INFO,
				"\nSuccessfully saved scene file '" + path(Engine::scenePath).parent_path().stem().string() + "'!\n");
		}

		string lastSavedScenePath = (path(Engine::docsPath) / "lastSavedScene.txt").string();
		if (exists(lastSavedScenePath)) File::DeleteTarget(lastSavedScenePath);
		ofstream lastSavedSceneFile(lastSavedScenePath);
		if (!lastSavedSceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Couldn't write into last saved scene file '" + Engine::scenePath + "'!\n");
		}
		else
		{
			string currentScene = Engine::scenePath;
			lastSavedSceneFile << currentScene;
		}
		lastSavedSceneFile.close();

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

	string SceneFile::GetKey(const string& key)
	{
		auto it = find(keys.begin(), keys.end(), key);
		return (it != keys.end()) ? *it : "";
	}
	string SceneFile::GetValue(const string& key, bool silent)
	{
		string foundKey = GetKey(key);
		if (foundKey == "")
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Cannot get scene file key " + key + " value because it does not exist!\n");
			return "";
		}
		else
		{
			auto it = find(keys.begin(), keys.end(), key);
			int pos = distance(keys.begin(), it);

			return values[pos];
		}
	}
	void SceneFile::SetValue(const string& key, const string& value)
	{
		string foundKey = GetKey(key);
		if (foundKey == "")
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Cannot get scene file key " + key + " value because it does not exist!\n");
		}
		else
		{
			auto it = find(keys.begin(), keys.end(), key);
			int pos = distance(keys.begin(), it);

			values[pos] = value;
		}
	}

	void SceneFile::CreateNewSceneFile(const string& newPath)
	{
		if (sceneFilePath == "") sceneFilePath = newPath;

		keys.clear();
		values.clear();

		SetDefaultData();

		ofstream sceneFile(sceneFilePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Couldn't write into scene file '" + sceneFilePath + "'!\n");
			return;
		}

		for (int i = 0; i < keys.size(); i++)
		{
			string key = keys[i];
			string value = values[i];

			sceneFile << key << "= " << value << "\n";
		}

		sceneFile.close();

		LoadScene(sceneFilePath);
	}

	void SceneFile::SetDefaultData()
	{
		cout << "set default data...\n";

		//
		// GLOBAL GRAPHICS DATA
		//

#if ENGINE_MODE
		if (Render::activeCamera != nullptr)
		{
			//sets camera default position and rotation if default scene is created
			auto tc = Render::activeCamera->GetComponent<TransformComponent>();
			tc->SetPosition(vec3(0.0f, 1.0f, 0.0f));
			tc->SetRotation(vec3(-90.0f, 0.0f, 0.0f));
		}

		keys.push_back("camera_position"); 
			values.push_back("0.0, 1.0, 0.0");
		keys.push_back("camera_rotation");
			values.push_back("0.0, 0.0, 0.0");
#endif

		string texturesFolder = (path(Engine::filesPath) / "textures").string();
		string skyboxDefault = (path(texturesFolder).filename() / "skybox_default.png").string();

		keys.push_back("skybox_right");
			values.push_back(skyboxDefault);
		keys.push_back("skybox_left");
			values.push_back(skyboxDefault);
		keys.push_back("skybox_top");
			values.push_back(skyboxDefault);
		keys.push_back("skybox_bottom");
			values.push_back(skyboxDefault);
		keys.push_back("skybox_front");
			values.push_back(skyboxDefault);
		keys.push_back("skybox_back");
			values.push_back(skyboxDefault);

		keys.push_back("globalAmbientColor");
			values.push_back("0.775, 0.775, 0.555");
		keys.push_back("globalAmbientIntensity");
			values.push_back("0.5");

		//
		// GLOBAL PHYSICS DATA
		//

		keys.push_back("gravity");
			values.push_back("0.0, -9.81, 0.0");
		keys.push_back("angleLimit");
			values.push_back("45.0");
		keys.push_back("angularDamping");
			values.push_back("0.5");
		keys.push_back("lowAngularVelocityFactor");
			values.push_back("0.5");
		keys.push_back("frictionMultiplier");
			values.push_back("0.1");
		keys.push_back("correctionFactor");
			values.push_back("0.2");
		keys.push_back("minPenetrationThreshold");
			values.push_back("0.01");
	}

	void SceneFile::SaveGlobalPhysicsData()
	{
		if (Render::activeCamera == nullptr) return;

		if (Physics::physicsWorld == nullptr) return;

		cout << "save global physics data!\n";

		vec3 gravity = Physics::physicsWorld->GetGravity();
		string gravityString =
			to_string(gravity.x) + ", " +
			to_string(gravity.y) + ", " +
			to_string(gravity.z);
		SetValue("gravity", gravityString);

		string angleLimit =
			to_string(Physics::physicsWorld->GetAngleLimit());
		SetValue("angleLimit", angleLimit);

		string angularDamping =
			to_string(Physics::physicsWorld->GetAngularDamping());
		SetValue("angularDamping", angularDamping);

		string lowAngularVelocityFactor =
			to_string(Physics::physicsWorld->GetLowAngularVelocityFactor());
		SetValue("lowAngularVelocityFactor", lowAngularVelocityFactor);

		string frictionMultiplier =
			to_string(Physics::physicsWorld->GetFrictionMultiplier());
		SetValue("frictionMultiplier", frictionMultiplier);

		string correctionFactor =
			to_string(Physics::physicsWorld->GetCorrectionFactor());
		SetValue("correctionFactor", correctionFactor);

		string minPenetrationThreshold =
			to_string(Physics::physicsWorld->GetMinPenetrationThreshold());
		SetValue("minPenetrationThreshold", minPenetrationThreshold);
	}
	void SceneFile::LoadGlobalPhysicsData()
	{
		if (Render::activeCamera == nullptr) return;

		if (Physics::physicsWorld == nullptr)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Tried to load global physics data but Elypso Physics has not yet been initialized!");
			return;
		}

		cout << "!!!! load global physics data!\n";
		
		string gravityString = GetValue("gravity");
		vector<string> split = String::Split(gravityString, ',');
		vec3 gravity = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
		Physics::physicsWorld->SetGravity(gravity);

		float angleLimit = stof(GetValue("angleLimit"));
		Physics::physicsWorld->SetAngleLimit(angleLimit);

		float angularDamping = stof(GetValue("angularDamping"));
		Physics::physicsWorld->SetAngularDamping(angularDamping);

		float lowAngularVelocityFactor = stof(GetValue("lowAngularVelocityFactor"));
		Physics::physicsWorld->SetLowAngularVelocityFactor(lowAngularVelocityFactor);

		float frictionMultiplier = stof(GetValue("frictionMultiplier"));
		Physics::physicsWorld->SetFrictionMultiplier(frictionMultiplier);

		float correctionFactor = stof(GetValue("correctionFactor"));
		Physics::physicsWorld->SetCorrectionFactor(correctionFactor);

		float minPenetrationThreshold = stof(GetValue("minPenetrationThreshold"));
		Physics::physicsWorld->SetMinPenetrationThreshold(minPenetrationThreshold);
	}

	void SceneFile::SaveGlobalGraphicsData()
	{
#if ENGINE_MODE
		if (Render::activeCamera == nullptr) return;

		auto tc = Render::activeCamera->GetComponent<TransformComponent>();
		vec3 pos = tc->GetPosition();
		string cameraPos =
			to_string(pos[0]) + ", " +
			to_string(pos[1]) + ", " +
			to_string(pos[2]);
		SetValue("camera_position", cameraPos);

		vec3 rot = tc->GetRotation();
		string cameraRot =
			to_string(rot[0]) + ", " +
			to_string(rot[1]) + ", " +
			to_string(rot[2]);
		SetValue("camera_rotation", cameraRot);
#endif

		cout << "!!!! save global graphics data!\n";

		SetValue("skybox_right", skyboxTexturesMap["right"]);
		SetValue("skybox_left", skyboxTexturesMap["left"]);
		SetValue("skybox_top", skyboxTexturesMap["top"]);
		SetValue("skybox_bottom", skyboxTexturesMap["bottom"]);
		SetValue("skybox_front", skyboxTexturesMap["front"]);
		SetValue("skybox_back", skyboxTexturesMap["back"]);

		vec3 globalAmbientColor = Render::globalAmbientColor;
		string globalAmbientColorString =
			to_string(globalAmbientColor.x) + ", " +
			to_string(globalAmbientColor.y) + ", " +
			to_string(globalAmbientColor.z);
		SetValue("globalAmbientColor", globalAmbientColorString);

		float globalAmbientIntensity = Render::globalAmbientIntensity;
		SetValue("globalAmbientIntensity", to_string(globalAmbientIntensity));
	}
	void SceneFile::LoadGlobalGraphicsData()
	{
#if ENGINE_MODE
		if (Render::activeCamera == nullptr)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Failed to load global graphics data because no camera was found!\n");
			return;
		}

		auto tc = Render::activeCamera->GetComponent<TransformComponent>();

		vector<string> cameraPosVector = String::Split(GetValue("camera_position"), ',');
		vec3 cameraPos = vec3(
			stof(cameraPosVector[0]),
			stof(cameraPosVector[1]),
			stof(cameraPosVector[2]));
		tc->SetPosition(cameraPos);

		vector<string> cameraRotVector = String::Split(GetValue("camera_rotation"), ',');
		vec3 cameraRot = vec3(
			stof(cameraRotVector[0]),
			stof(cameraRotVector[1]),
			stof(cameraRotVector[2]));
		tc->SetRotation(cameraRot);
#endif

		cout << "!!!! load global graphics data!\n";

		string texturesFolder = (path(Engine::filesPath) / "textures").string();
		string skyboxDefault = (path(texturesFolder).filename() / "skybox_default.png").string();
		vector<string> skyboxTextures;

		string skybox_right = GetValue("skybox_right") != ""
			? GetValue("skybox_right") : skyboxDefault;
		skyboxTextures.push_back(skybox_right);
		skyboxTexturesMap["right"] = skybox_right;

		string skybox_left = GetValue("skybox_left") != ""
			? GetValue("skybox_left") : skyboxDefault;
		skyboxTextures.push_back(skybox_left);
		skyboxTexturesMap["left"] = skybox_left;

		string skybox_top = GetValue("skybox_top") != ""
			? GetValue("skybox_top") : skyboxDefault;
		skyboxTextures.push_back(skybox_top);
		skyboxTexturesMap["top"] = skybox_top;

		string skybox_bottom = GetValue("skybox_bottom") != ""
			? GetValue("skybox_bottom") : skyboxDefault;
		skyboxTextures.push_back(skybox_bottom);
		skyboxTexturesMap["bottom"] = skybox_bottom;

		string skybox_front = GetValue("skybox_front") != ""
			? GetValue("skybox_front") : skyboxDefault;
		skyboxTextures.push_back(skybox_front);
		skyboxTexturesMap["front"] = skybox_front;

		string skybox_back = GetValue("skybox_back") != ""
			? GetValue("skybox_back") : skyboxDefault;
		skyboxTextures.push_back(skybox_back);
		skyboxTexturesMap["back"] = skybox_back;

		if (skyboxTextures.empty())
		{
			skyboxTextures.push_back(skyboxDefault);
			skyboxTextures.push_back(skyboxDefault);
			skyboxTextures.push_back(skyboxDefault);
			skyboxTextures.push_back(skyboxDefault);
			skyboxTextures.push_back(skyboxDefault);
			skyboxTextures.push_back(skyboxDefault);

			skyboxTexturesMap["right"] = skyboxDefault;
			skyboxTexturesMap["left"] = skyboxDefault;
			skyboxTexturesMap["top"] = skyboxDefault;
			skyboxTexturesMap["bottom"] = skyboxDefault;
			skyboxTexturesMap["front"] = skyboxDefault;
			skyboxTexturesMap["back"] = skyboxDefault;
		}
		Skybox::AssignSkyboxTextures(skyboxTextures, false);

		string globalAmbientColorString = GetValue("globalAmbientColor");
		vector<string> split = String::Split(globalAmbientColorString, ',');
		vec3 globalAmbientColor = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
		Render::globalAmbientColor = globalAmbientColor;

		string globalAmbientIntensityString = GetValue("globalAmbientIntensity");
		Render::globalAmbientIntensity = stof(globalAmbientIntensityString);
	}
}
