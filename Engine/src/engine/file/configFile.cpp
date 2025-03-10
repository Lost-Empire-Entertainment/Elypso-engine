//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>
#include <iostream>

//external
#include "glm.hpp"
#include "glfw3.h"
#include "imgui_impl_glfw.h"

//engine
#include "configFile.hpp"
#include "core.hpp"
#include "console.hpp"
#include "fileUtils.hpp"
#include "sceneFile.hpp"
#include "render.hpp"
#include "stringUtils.hpp"
#include "physics.hpp"
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
using Utils::File;
using Graphics::Render;
using Utils::String;
using Core::Physics;
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
				}
			}

			configFile.close();

			LoadGlobalPhysicsData();

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
			File::DeleteFileOrfolder(configFilePath);
		}

		SaveGlobalPhysicsData();

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

		Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"\nSuccessfully saved config file!\n");
	}

	string ConfigFile::GetValue(const string& key, bool silent)
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
			if (!silent)
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::FILE,
					Type::EXCEPTION,
					"Error: Cannot get config key " + key + " value because it does not exist! This will cause a crash if the config file was not filled correctly.\n");
			}
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
				ImGui::GetIO().FontGlobalScale = stof(value);
			}
		}
		else
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Cannot set value to config key " + key + " because it does not exist!\n");
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

		keys.push_back("camera_speedMultiplier");
			values.push_back("1.0");
		keys.push_back("camera_fov");
			values.push_back("90.0");
		keys.push_back("camera_nearClip");
			values.push_back("0.001");
		keys.push_back("camera_farClip");
			values.push_back("200.0");

		keys.push_back("gravity");
			values.push_back("0.0, -9.81, 0.0");
		keys.push_back("angularDamping");
			values.push_back("0.998");
		keys.push_back("lowAngularVelocityFactor");
			values.push_back("0.5");
		keys.push_back("frictionMultiplier");
			values.push_back("0.1");
		keys.push_back("correctionFactor");
			values.push_back("0.2");
		keys.push_back("minPenetrationThreshold");
			values.push_back("0.01");
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

	void ConfigFile::SaveGlobalPhysicsData()
	{
		if (Physics::physicsWorld != nullptr)
		{
			vec3 gravity = Physics::physicsWorld->GetGravity();
			string gravityString =
				to_string(gravity.x) + ", " +
				to_string(gravity.y) + ", " +
				to_string(gravity.z);
			SetValue("gravity", gravityString);

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
	}
	void ConfigFile::LoadGlobalPhysicsData()
	{
		if (Physics::physicsWorld != nullptr)
		{
			string gravityString = GetValue("gravity");
			vector<string> split = String::Split(gravityString, ',');
			vec3 gravity = vec3(stof(split[0]), stof(split[1]), stof(split[2]));
			Physics::physicsWorld->SetGravity(gravity);

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
	}
}