//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>

//external
#include "glm.hpp"
#include "glfw3.h"
#include "imgui_impl_glfw.h"
#include "stringutils.hpp"
#include "fileutils.hpp"

//engine
#include "configFile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "sceneFile.hpp"
#include "render.hpp"
#include "gameobject.hpp"
#if ENGINE_MODE
#include "gui_settings.hpp"
#endif

using std::ofstream;
using std::ifstream;
using std::filesystem::exists;
using glm::vec3;
using std::ios;
using std::filesystem::path;
using std::filesystem::current_path;
using std::cout;
using std::to_string;

using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Graphics::Render;
using Graphics::Shape::GameObjectManager;
using KalaKit::FileUtils;
using KalaKit::StringUtils;
#if ENGINE_MODE
using Graphics::GUI::GUISettings;
#endif

namespace EngineFile
{
	void ConfigFile::LoadConfigFile()
	{
#if ENGINE_MODE
		if (configFilePath == "") configFilePath = (path(Engine::docsPath) / "config.txt").string();
#else
		path currentPath = current_path();
		if (configFilePath == "") configFilePath = (currentPath / "config.txt").string();
#endif

		if (!exists(configFilePath)) CreateNewConfigFile();
		else 
		{
			//opens file in binary mode and moves to end
			//if file is empty then a new one is created
			ifstream configFileBinary(configFilePath, ios::binary | ios::ate); 
			if (configFileBinary.tellg() == 0)
			{
				configFileBinary.close();

				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::INFO,
					"Failed to load config file '" + configFilePath + "' because it was empty! Creating a new one.\n\n");

				CreateNewConfigFile();
				return;
			}

			ifstream configFile(configFilePath);
			if (!configFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: Failed to open config file '" + configFilePath + "'!\n\n");
				return;
			}

			keys.clear();
			values.clear();

			string line;
			map<string, string> configValue;
			while (getline(configFile, line))
			{
				if (!line.empty()
					&& line.find("=") != string::npos)
				{
					vector<string> splitLine = StringUtils::Split(line, '=');
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

					keys.push_back(key);
					values.push_back(value);
				}
			}

			configFile.close();

			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::DEBUG,
				"Successfully loaded config file!\n");
		}
	}

	void ConfigFile::SaveConfigFile()
	{
		if (exists(configFilePath))
		{
			FileUtils::DeleteTarget(configFilePath);
		}

		ofstream configFile(configFilePath);

#if ENGINE_MODE
		SetValue("renderBorders", to_string(GameObjectManager::renderBorders));
		SetValue("renderPointLightBillboards", to_string(GameObjectManager::renderPointLightBillboards));
		SetValue("renderSpotlightBillboards", to_string(GameObjectManager::renderSpotlightBillboards));
		SetValue("renderDirLightBillboard", to_string(GameObjectManager::renderDirLightBillboard));
		SetValue("renderAudioObjectBillboards", to_string(GameObjectManager::renderAudioObjectBillboards));
		SetValue("renderCameraObjectBillboards", to_string(GameObjectManager::renderCameraObjectBillboards));
#endif

		if (!configFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Couldn't write into config file '" + configFilePath + "'!\n");
			return;
		}

		for (int i = 0; i < keys.size(); i++)
		{
			string key = keys[i];
			string value = values[i];

			configFile << key << "= " << value << "\n";
		}

		configFile.close();

		Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"\nSuccessfully saved config file!\n");
	}

	string ConfigFile::GetKey(const string& key)
	{
		auto it = find(keys.begin(), keys.end(), key);
		return (it != keys.end()) ? *it : "";
	}
	string ConfigFile::GetValue(const string& key, bool silent)
	{
		string foundKey = GetKey(key);
		if (foundKey == "")
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Cannot get config file key " + key + " value because it does not exist!\n");
			return "";
		}
		else
		{
			auto it = find(keys.begin(), keys.end(), key);
			int pos = distance(keys.begin(), it);

			return values[pos];
		}
	}
	void ConfigFile::SetValue(const string& key, const string& value)
	{
		string foundKey = GetKey(key);
		if (foundKey == "")
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Cannot get config file key " + key + " value because it does not exist!\n");
		}
		else
		{
			auto it = find(keys.begin(), keys.end(), key);
			int pos = distance(keys.begin(), it);

			values[pos] = value;
		}
	}

	void ConfigFile::CreateNewConfigFile()
	{
#if ENGINE_MODE
		if (configFilePath == "") configFilePath = (path(Engine::docsPath) / "config.txt").string();
#else
		path currentPath = current_path();
		if (configFilePath == "") configFilePath = (currentPath / "config.txt").string();
#endif

		keys.clear();
		values.clear();

#if ENGINE_MODE
		keys.push_back("gameName");
			values.push_back("Game");
#endif
		keys.push_back("gui_fontScale");
			values.push_back("1.5");

		keys.push_back("window_vsync");
			values.push_back("1");

		keys.push_back("aspect_ratio");
			values.push_back("1");
#if ENGINE_MODE
		keys.push_back("grid_color");
			values.push_back("0.4, 0.4, 0.4");
		keys.push_back("grid_transparency");
			values.push_back("0.25");
		keys.push_back("grid_maxDistance");
			values.push_back("50.0");

		keys.push_back("gui_sceneWindow");
			values.push_back("1");
		keys.push_back("gui_inspector");
			values.push_back("1");
		keys.push_back("gui_sceneHierarchy");
			values.push_back("1");
		keys.push_back("gui_projectHierarchy");
			values.push_back("1");
#endif
		keys.push_back("gui_console");
			values.push_back("1");

#if ENGINE_MODE
		keys.push_back("renderBorders");
			values.push_back("1");
		keys.push_back("renderPointLightBillboards");
			values.push_back("1");
		keys.push_back("renderSpotlightBillboards");
			values.push_back("1");
		keys.push_back("renderDirLightBillboard");
			values.push_back("1");
		keys.push_back("renderAudioObjectBillboards");
			values.push_back("1");
		keys.push_back("renderCameraObjectBillboards");
			values.push_back("1");
#endif

		ofstream configFile(configFilePath);

		if (!configFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Couldn't write into config file '" + configFilePath + "'!\n");
			return;
		}

		for (int i = 0; i < keys.size(); i++)
		{
			string key = keys[i];
			string value = values[i];

			configFile << key << "= " << value << "\n";
		}

		configFile.close();

		LoadConfigFile();
	}
}