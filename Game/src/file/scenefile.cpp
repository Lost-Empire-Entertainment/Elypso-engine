//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <fstream>
#include <vector>

//game
#include "scenefile.hpp"
#include "core.hpp"
#include "gameobject.hpp"
#include "console.hpp"
#include "stringutils.hpp"
#include "render.hpp"
#include "fileutils.hpp"
#include "gameobjectfile.hpp"

using std::filesystem::exists;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::filesystem::path;

using Core::Game;
using Graphics::Shape::GameObjectManager;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::String;
using Graphics::Render;
using Utils::File;

namespace GameFile
{
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

		Game::scenePath = scenePath;
		Game::currentGameobjectsPath = path(Game::scenePath).parent_path().string() + "\\gameobjects";

		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
		if (objects.size() != 0)
		{
			for (const auto& obj : objects)
			{
				GameObjectManager::DestroyGameObject(obj, true);
			}
		}

		ifstream sceneFile(Game::scenePath);
		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Failed to open scene file '" + Game::scenePath + "'!\n\n");
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

		GameObjectFile::LoadGameObjects(Game::currentGameobjectsPath);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::INFO,
			"Successfully loaded scene file '" + path(Game::scenePath).parent_path().stem().string() + "'!\n");
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

		ofstream sceneFile(Game::scenePath);

		if (!sceneFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Couldn't write into scene file '" + Game::scenePath + "'!\n");
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

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::INFO,
			"\nSuccessfully saved scene file '" + path(Game::scenePath).parent_path().stem().string() + "'!\n");

		switch (saveType)
		{
		case SaveType::sceneSwitch:
			LoadScene(targetLevel);
			break;
		case SaveType::shutDown:
			Game::Shutdown();
			break;
		}
	}
}