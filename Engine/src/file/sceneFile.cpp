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
#include "gameObjectFile.hpp"

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
using EngineFile::GameObjectFile;

namespace EngineFile
{
	void SceneFile::CheckForProjectFile()
	{
		string projectPath = Engine::filesPath + "\\project.txt";
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
			Engine::CreateErrorPopup("Project load error", "Failed to load valid project file from project file! Shutting down engine");
		}

		if (targetScene.empty()
			|| !exists(targetScene))
		{
			Engine::CreateErrorPopup("Scene load error", "Failed to load valid scene file from project file! Shutting down engine");
		}

		Engine::scenePath = targetScene;
	}

	void SceneFile::LoadScene(const string& scenePath)
	{
		if (!exists(scenePath))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Tried to load scene file '" + scenePath + "' but it doesn't exist!\n");
			return;
		}
		else
		{
			Engine::scenePath = scenePath;
			Engine::currentGameobjectsPath = path(Engine::scenePath).parent_path().string() + "\\gameobjects";
			Engine::texturesPath = path(Engine::projectPath).parent_path().string() + "\\textures";
			Engine::scenesPath = path(Engine::projectPath).parent_path().string() + "\\scenes";
		}

		//create textures folder if it doesnt exist
		if (!exists(Engine::texturesPath)) File::CreateNewFolder(Engine::texturesPath);

		Select::isObjectSelected = false;
		Select::selectedObj = nullptr;

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
			}
		}

		sceneFile.close();

		GameObjectFile::LoadGameObjects(Engine::currentGameobjectsPath);

		if (unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::INFO,
			"Successfully loaded scene file '" + path(Engine::scenePath).parent_path().stem().string() + "'!\n");
	}

	void SceneFile::SaveScene(SaveType saveType, const string& targetLevel)
	{
		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();

		for (const auto& obj : objects)
		{
			if (obj->GetParentBillboardHolder() == nullptr)
			{
				GameObjectFile::SaveGameObject(obj);
			}
		}

		ofstream sceneFile(Engine::scenePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Couldn't write into scene file '" + Engine::scenePath + "'!\n");
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

		sceneFile.close();

		if (unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::INFO,
			"\nSuccessfully saved scene file '" + path(Engine::scenePath).parent_path().stem().string() + "'!\n");

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