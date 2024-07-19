//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>

//engine
#include "configFile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "fileUtils.hpp"
#include "sceneFile.hpp"
#include "render.hpp"
#include "stringUtils.hpp"

using std::ofstream;
using std::ifstream;
using std::filesystem::exists;

using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Utils::File;
using Graphics::Render;
using Utils::String;

namespace EngineFile
{
	void ConfigFile::LoadConfigFile()
	{
		if (configFilePath == "") configFilePath = Engine::docsPath + "\\config.txt";

		if (!exists(configFilePath)) CreateNewConfigFile();
		else 
		{
			ifstream configFile(configFilePath);
			if (!configFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Failed to open config file '" + configFilePath + "'!\n\n");
				return;
			}

			configValues.clear();

			string line;
			map<string, string> configValue;
			while (getline(configFile, line))
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

					configValues[type] = value;
				}
			}

			configFile.close();

			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"Successfully loaded config file '" + configFilePath + "'!\n");
		}
	}

	void ConfigFile::SaveConfigFile()
	{
		if (exists(configFilePath))
		{
			File::DeleteFileOrfolder(configFilePath);
		}

		ofstream configFile(configFilePath);

		if (!configFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't write into config file '" + configFilePath + "'!\n");
			return;
		}

		for (const auto& kvp : configValues)
		{
			string key = kvp.first;
			string value = kvp.second;

			configFile << key << "= " << value << "\n";
		}

		configFile.close();

		if (SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"\nSuccessfully created new config file '" + configFilePath + "'!\n");
	}

	string ConfigFile::GetValue(const string& key)
	{
		if (configValues.find(key) != configValues.end())
		{
			return configValues[key];
		}
		else 
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Cannot get config key " + key + " value because it does not exist! This will cause a crash if the config file was not filled correctly.\n");

			return "";
		}
	}

	void ConfigFile::SetValue(const string& key, const string& value)
	{
		if (configValues.find(key) != configValues.end())
		{
			configValues[key] = value;
		}
		else
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Cannot set value to config key " + key + " because it does not exist!\n");
		}
	}

	void ConfigFile::CreateNewConfigFile()
	{
		configValues.clear();

		configValues["gui_fontScale"] = "1.5";

		configValues["window_vsync"] = "1";
		configValues["camera_position"] = "0.0, 1.0, 0.0";
		configValues["camera_rotation"] = "-90.00, 0.00, 0.00";

		configValues["gui_inspector"] = "1";
		configValues["gui_console"] = "1";
		configValues["gui_sceneHierarchy"] = "1";
		configValues["gui_projectHierarchy"] = "1";
		configValues["gui_sceneMenu"] = "0";

		ofstream configFile(configFilePath);

		if (!configFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't write into config file '" + configFilePath + "'!\n");
			return;
		}

		for (const auto& kvp : configValues)
		{
			string key = kvp.first;
			string value = kvp.second;

			configFile << key << "= " << value << "\n";
		}

		configFile.close();

		LoadConfigFile();
	}
}