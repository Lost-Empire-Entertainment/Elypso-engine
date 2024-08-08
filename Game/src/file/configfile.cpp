//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>
#include <map>

//external
#include "glm.hpp"

//game
#include "configfile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "stringutils.hpp"
#include "render.hpp"
#include "fileutils.hpp"

using std::ifstream;
using glm::vec3;
using std::to_string;
using std::ofstream;
using std::map;
using std::filesystem::exists;
using std::ios;

using Core::Game;
using Core::Console;
using Caller = Core::Console::Caller;
using Type = Core::Console::Type;
using Utils::String;
using Graphics::Render;
using Utils::File;

namespace GameFile
{
	void ConfigFile::LoadConfigFile()
	{
		if (configFilePath == "") configFilePath = Game::docsPath + "\\config.txt";

		if (!exists(configFilePath)) CreateNewConfigFile();
		else
		{
			//opens file in binary mode and moves to end
			//if file is empty then a new one is created
			ifstream configFileBinary(configFilePath, ios::binary | ios::ate);
			if (configFileBinary.tellg() == 0)
			{
				configFileBinary.close();

				Console::WriteConsoleMessage(
					Caller::FILE,
					Type::INFO,
					"Failed to load config file '" + configFilePath + "' because it was empty! Creating a new one.\n\n");

				CreateNewConfigFile();
				return;
			}

			ifstream configFile(configFilePath);
			if (!configFile.is_open())
			{
				Console::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Failed to open config file '" + configFilePath + "'!\n\n");
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

					keys.push_back(key);
					values.push_back(value);

					if (key == "camera_position")
					{
						vector<string> posSplit = String::Split(value, ',');
						vec3 pos = vec3(
							stof(posSplit[0]),
							stof(posSplit[1]),
							stof(posSplit[2]));
						Render::camera.SetCameraPosition(pos);
					}
					else if (key == "camera_rotation")
					{
						vector<string> rotSplit = String::Split(value, ',');
						vec3 rot = vec3(
							stof(rotSplit[0]),
							stof(rotSplit[1]),
							stof(rotSplit[2]));
						Render::camera.SetCameraRotation(rot);
					}
				}
			}

			configFile.close();

			Console::WriteConsoleMessage(
				Caller::FILE,
				Type::DEBUG,
				"Successfully loaded config file!\n");
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
			Console::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Couldn't write into config file '" + configFilePath + "'!\n");
			return;
		}

		for (int i = 0; i < keys.size(); i++)
		{
			string key = keys[i];
			string value = values[i];

			if (key == ".")
			{
				configFile << "\n";
			}
			else if (key == "-")
			{
				configFile << "-------------------------\n";
			}
			else
			{
				if (key == "camera_position")
				{
					vec3 pos = Render::camera.GetCameraPosition();
					value =
						to_string(pos[0]) + "," +
						to_string(pos[1]) + "," +
						to_string(pos[2]);
				}
				else if (key == "camera_rotation")
				{
					vec3 rot = Render::camera.GetCameraRotation();
					value =
						to_string(rot[0]) + "," +
						to_string(rot[1]) + "," +
						to_string(rot[2]);
				}

				configFile << key << "= " << value << "\n";
			}
		}

		configFile.close();

		Console::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"\nSuccessfully saved config file!\n");
	}

	string ConfigFile::GetValue(const string& key)
	{
		int keyIndex = -1;
		for (int i = 0; i < keys.size(); i++)
		{
			if (keys[i] == key)
			{
				keyIndex = i;
				break;
			}
		}

		if (keyIndex != -1)
		{
			return values[keyIndex];
		}
		else
		{
			Console::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Cannot get config key " + key + " value because it does not exist! This will cause a crash if the config file was not filled correctly.\n");

			return "";
		}
	}

	void ConfigFile::SetValue(const string& key, const string& value)
	{
		int keyIndex = -1;
		for (int i = 0; i < keys.size(); i++)
		{
			if (keys[i] == key)
			{
				keyIndex = i;
				break;
			}
		}

		if (keyIndex != -1)
		{
			values[keyIndex] = value;

			if (key == "gui_fontScale")
			{
				//ImGui::GetIO().FontGlobalScale = stof(value);
			}
		}
		else
		{
			Console::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Cannot set value to config key " + key + " because it does not exist!\n");
		}
	}

	void ConfigFile::CreateNewConfigFile()
	{
		keys.clear();
		values.clear();

		//
		// CONFIG VALUES EDITABLE THROUGH ENGINE
		//

		keys.push_back("gui_fontScale");
		values.push_back("1.5");

		keys.push_back("window_vsync");
		values.push_back("1");

		keys.push_back("camera_speedMultiplier");
		values.push_back("1.0");
		keys.push_back("camera_fov");
		values.push_back("90.0");
		keys.push_back("camera_nearClip");
		values.push_back("0.001");
		keys.push_back("camera_farClip");
		values.push_back("200.0");
		keys.push_back("camera_position");
		values.push_back("0.0, 1.0, 0.0");
		keys.push_back("camera_rotation");
		values.push_back("-90.00, 0.00, 0.00");

		keys.push_back("grid_color");
		values.push_back("0.4, 0.4, 0.4");
		keys.push_back("grid_transparency");
		values.push_back("0.25");
		keys.push_back("grid_maxDistance");
		values.push_back("50.0");

		keys.push_back("gui_inspector");
		values.push_back("1");
		keys.push_back("gui_console");
		values.push_back("1");
		keys.push_back("gui_sceneHierarchy");
		values.push_back("1");
		keys.push_back("gui_projectHierarchy");
		values.push_back("1");
		keys.push_back("gui_sceneMenu");
		values.push_back("0");

		ofstream configFile(configFilePath);

		if (!configFile.is_open())
		{
			Console::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Couldn't write into config file '" + configFilePath + "'!\n");
			return;
		}

		for (int i = 0; i < keys.size(); i++)
		{
			string key = keys[i];
			string value = values[i];

			if (key == ".")
			{
				configFile << "\n";
			}
			else if (key == "-")
			{
				configFile << "-------------------------\n";
			}
			else
			{
				configFile << key << "= " << value << "\n";
			}
		}

		configFile.close();

		LoadConfigFile();
	}
}