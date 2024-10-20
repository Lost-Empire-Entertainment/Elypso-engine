//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

//external
#include "magic_enum.hpp"
#include "glm.hpp"

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
#include "gameObjectFile.hpp"
#include "skybox.hpp"

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

using Core::Engine;
using Core::Select;
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
using EngineFile::GameObjectFile;
using Graphics::Shape::Skybox;

namespace EngineFile
{
	void SceneFile::LoadScene(const string& scenePath)
	{
		if (!exists(scenePath))
		{
			if (scenePath != Engine::scenesPath + "\\Scene1\\scene.txt")
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: Tried to load scene file '" + scenePath + "' but it doesn't exist! Loading default scene.\n");
			}

			//create new default scene folder if it doesnt exist
			string defaultSceneFolder = Engine::scenesPath + "\\Scene1";
			if (!exists(defaultSceneFolder))
			{
				File::CreateNewFolder(defaultSceneFolder);
				File::CreateNewFolder(defaultSceneFolder + "\\gameobjects");

				//create new default scene file
				string defaultScenePath = defaultSceneFolder + "\\scene.txt";
				ofstream defaultSceneFile(defaultScenePath);
				if (!defaultSceneFile.is_open())
				{
					Engine::CreateErrorPopup(
						"Failed to create default scene file!");
				}
				defaultSceneFile.close();

				//sets camera default position and rotation if default scene is created
				Render::camera.SetCameraPosition(vec3(0.0f, 1.0f, 0.0f));
				Render::camera.SetCameraRotation(vec3(-90.0f, 0.0f, 0.0f));
			}

			//create new default scene file if it doesnt exist
			string defaultScenePath = defaultSceneFolder + "\\scene.txt";
			if (!exists(defaultScenePath))
			{
				ofstream defaultSceneFile(defaultScenePath);
				if (!defaultSceneFile.is_open())
				{
					Engine::CreateErrorPopup(
						"Failed to create default scene file!");
				}
				defaultSceneFile.close();
			}

			LoadScene(defaultScenePath);
		}
		else 
		{
			Engine::scenePath = scenePath;
			Engine::currentGameobjectsPath = path(Engine::scenePath).parent_path().string() + "\\gameobjects";

			vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
			if (objects.size() != 0)
			{
				for (const auto& obj : objects)
				{
					GameObjectManager::DestroyGameObject(obj, true);
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

			string texturesFolder = Engine::filesPath + "\\textures";
			string skyboxDefault = texturesFolder + "\\skybox_default.png";
			vector<string> skyboxTextures;

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

					vector<string> splitValue;
					if (value.find(',') != string::npos)
					{
						splitValue = String::Split(value, ',');
					}

					if (type == "camera_position")
					{
						vec3 cameraPos = vec3(
							stof(splitValue[0]),
							stof(splitValue[1]),
							stof(splitValue[2]));
						Render::camera.SetCameraPosition(cameraPos);
					}
					else if (type == "camera_rotation")
					{
						vec3 cameraRot = vec3(
							stof(splitValue[0]),
							stof(splitValue[1]),
							stof(splitValue[2]));
						Render::camera.SetCameraRotation(cameraRot);
					}

					else if (type == "skybox_right")
					{
						string finalValue = value != ""
							? value
							: skyboxDefault;

						skyboxTextures.push_back(finalValue);
						skyboxTexturesMap["right"] = finalValue;
					}
					else if (type == "skybox_left")
					{
						string finalValue = value != ""
							? value
							: skyboxDefault;

						skyboxTextures.push_back(finalValue);
						skyboxTexturesMap["left"] = finalValue;
					}
					else if (type == "skybox_top")
					{
						string finalValue = value != ""
							? value
							: skyboxDefault;

						skyboxTextures.push_back(finalValue);
						skyboxTexturesMap["top"] = finalValue;
					}
					else if (type == "skybox_bottom")
					{
						string finalValue = value != ""
							? value
							: skyboxDefault;

						skyboxTextures.push_back(finalValue);
						skyboxTexturesMap["bottom"] = finalValue;
					}
					else if (type == "skybox_front")
					{
						string finalValue = value != ""
							? value
							: skyboxDefault;

						skyboxTextures.push_back(finalValue);
						skyboxTexturesMap["front"] = finalValue;
					}
					else if (type == "skybox_back")
					{
						string finalValue = value != ""
							? value
							: skyboxDefault;

						skyboxTextures.push_back(finalValue);
						skyboxTexturesMap["back"] = finalValue;
					}
				}
			}

			sceneFile.close();

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

			GameObjectFile::LoadGameObjects();

			Render::SetWindowNameAsUnsaved(false);

			//update project file originating from hub 
			//to ensure currently opened scene is always opened when hub opens engine
			string projectFilePath = Engine::docsPath + "\\project.txt";
			File::DeleteFileOrfolder(projectFilePath);
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
#if DISCORD_MODE
			string exeName = Engine::name;
			string sceneName = path(Engine::scenePath).parent_path().stem().string();
			Engine::SetDiscordRichPresence(exeName, "Loaded scene " + sceneName, 0, 0, "icon");
#endif
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::INFO,
				"Successfully loaded scene file '" + path(Engine::scenePath).parent_path().stem().string() + "'!\n");
		}

		Select::selectedObj = nullptr;
		Select::isObjectSelected = false;
	}

	void SceneFile::SaveScene(SaveType saveType, const string& targetLevel)
	{
		GameObjectFile::SaveGameObjects();

		ofstream sceneFile(Engine::scenePath);
		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Couldn't write into scene file '" + Engine::scenePath + "'!\n");
			return;
		}

		vec3 pos = Render::camera.GetCameraPosition();
		string cameraPos =
			to_string(pos[0]) + "," +
			to_string(pos[1]) + "," +
			to_string(pos[2]);
		sceneFile << "camera_position= " << cameraPos << "\n";

		vec3 rot = Render::camera.GetCameraRotation();
		string cameraRot =
			to_string(rot[0]) + "," +
			to_string(rot[1]) + "," +
			to_string(rot[2]);
		sceneFile << "camera_rotation= " << cameraRot << "\n";

		sceneFile << "skybox_right= " << skyboxTexturesMap["right"] << "\n";
		sceneFile << "skybox_left= " << skyboxTexturesMap["left"] << "\n";
		sceneFile << "skybox_top= " << skyboxTexturesMap["top"] << "\n";
		sceneFile << "skybox_bottom= " << skyboxTexturesMap["bottom"] << "\n";
		sceneFile << "skybox_front= " << skyboxTexturesMap["front"] << "\n";
		sceneFile << "skybox_back= " << skyboxTexturesMap["back"] << "\n";

		sceneFile.close();

		Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::INFO,
			"\nSuccessfully saved scene file '" + path(Engine::scenePath).parent_path().stem().string() + "'!\n");

		string lastSavedScenePath = Engine::docsPath + "\\lastSavedScene.txt";
		if (exists(lastSavedScenePath)) File::DeleteFileOrfolder(lastSavedScenePath);
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
}