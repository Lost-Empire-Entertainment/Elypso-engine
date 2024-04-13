//Copyright(C) < 2024 > < Lost Empire Entertainment >

#include <fstream>
#include <filesystem>
#include <algorithm>

//external
#include "glm.hpp"

//game
#include "configFile.hpp"
#include "render.hpp"
#include "stringutils.hpp"
#include "core.hpp"
#include "input.hpp"
#include "sceneFile.hpp"

using glm::vec3;
using std::endl;
using std::cerr;
using std::to_string;
using std::ifstream;
using std::ofstream;
using std::exception;
using std::find_if;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::current_path;
using std::filesystem::create_directory;

using GameFile::SceneFile;
using Core::Input;
using Core::Game;
using Graphics::Render;
using Utils::String;

namespace GameFile
{
	void ConfigFileManager::SetDefaultConfigValues()
	{
		Render::windowWidth = 1280;
		Render::windowHeight = 720;
		Render::useMonitorRefreshRate = true;
		Input::moveSpeedMultiplier = 1.0f;
		Input::nearClip = 0.001f;
		Input::farClip = 100.0f;
		vec3 camPos = vec3(0.0f, 1.0f, 0.0f);
		Render::camera.SetCameraPosition(camPos);

		UpdateValues();
	}

	void ConfigFileManager::UpdateValues()
	{
		values.clear();

		int width, height;
		glfwGetWindowSize(Render::window, &width, &height);
		string finalWidth = width == 0 ? to_string(Render::windowWidth) : to_string(width);
		string finalHeight = height == 0 ? to_string(Render::windowHeight) : to_string(height);
		ConfigFileValue graphics_resolution(
			"window_resolution",
			finalWidth + ", " + finalHeight,
			"1280, 720",
			"7860, 3840",
			ConfigFileValue::Type::type_vec2);
		AddValue(graphics_resolution);

		ConfigFileValue graphics_vsync(
			"window_vsync",
			to_string(Render::useMonitorRefreshRate),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(graphics_vsync);

		ConfigFileValue camera_fov(
			"camera_fov",
			to_string(Input::fov),
			"70",
			"110",
			ConfigFileValue::Type::type_float);
		AddValue(camera_fov);

		ConfigFileValue camera_nearClip(
			"camera_nearClip",
			to_string(Input::nearClip),
			"0.001",
			"10000.0",
			ConfigFileValue::Type::type_float);
		AddValue(camera_nearClip);

		ConfigFileValue camera_farClip(
			"camera_farClip",
			to_string(Input::farClip),
			"0.001",
			"10000.0",
			ConfigFileValue::Type::type_float);
		AddValue(camera_farClip);

		string camera_position_value =
			to_string(Render::camera.GetCameraPosition().x) + ", " +
			to_string(Render::camera.GetCameraPosition().y) + ", " +
			to_string(Render::camera.GetCameraPosition().z);
		ConfigFileValue camera_position(
			"camera_position",
			camera_position_value,
			"-1000000.0, -1000000.0, -1000000.0",
			"1000000.0, 1000000.0, 1000000.0",
			ConfigFileValue::Type::type_vec3);
		AddValue(camera_position);

		string camera_rotation_value =
			to_string(Render::camera.GetCameraRotation().x) + ", " +
			to_string(Render::camera.GetCameraRotation().y) + ", " +
			to_string(Render::camera.GetCameraRotation().z);
		ConfigFileValue camera_rotation(
			"camera_rotation",
			camera_rotation_value,
			"-359.99, -359.99, -359.99",
			"359.99, 359.99, 359.99",
			ConfigFileValue::Type::type_vec3);
		AddValue(camera_rotation);
	}

	void ConfigFileManager::LoadConfigFile()
	{
		if (configFilePath == "")
		{
			configFilePath = Game::docsPath + "/config.txt";

			if (!exists(configFilePath))
			{
				SetDefaultConfigValues();
				CreateNewConfigFile();
			}
		}

		ifstream configFile(configFilePath);

		if (!configFile)
		{
			string title = "Failed to open config file";
			string description = "Couldn't open config file at " + configFilePath + "!";
			Game::CreateErrorPopup(title.c_str(), description.c_str());
		}

		string line;
		while (getline(configFile, line))
		{
			if (line.find(':') != string::npos)
			{
				line.erase(remove(line.begin(), line.end(), ' '), line.end());
				vector<string> lineSplit = String::Split(line, ':');
				vector<string> lineVariables;

				string name = lineSplit[0];
				string variables = lineSplit[1];
				if (variables.find(',') != string::npos)
				{
					variables = lineSplit[1];
					lineVariables = String::Split(variables, ',');
				}
				else lineVariables.push_back(lineSplit[1]);

				if (name == "window_resolution")
				{
					if (ConfigFileManager::IsValueInRange("width", lineVariables[0])
						&& ConfigFileManager::IsValueInRange("height", lineVariables[1]))
					{
						unsigned int width = stoul(lineVariables[0]);
						Render::windowWidth = width;
						unsigned int height = stoul(lineVariables[1]);
						Render::windowHeight = height;
						glfwSetWindowSize(Render::window, width, height);

						cout << "Set resolution to "
							<< to_string(Render::windowWidth) << ", "
							<< to_string(Render::windowHeight) << ".\n";
					}
					else
					{
						glfwSetWindowSize(Render::window, 1280, 720);

						cout << "Error: Height or width value " << lineVariables[0]
							<< " for resolution is out of range or not a float! Resetting to default.\n";
					}
				}
				else if (name == "window_vsync")
				{
					if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
					{
						Render::useMonitorRefreshRate = static_cast<bool>(stoi(lineVariables[0]));
						glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);

						cout << "Set vsync to " << to_string(Render::useMonitorRefreshRate) << ".\n";
					}
					else
					{
						Render::useMonitorRefreshRate = true;
						glfwSwapInterval(1);

						cout << "Error: VSync value " << lineVariables[0] << " is out of range or not an int! Resetting to default.\n";
					}
				}
				else if (name == "camera_fov")
				{
					if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
					{
						Input::fov = stof(lineVariables[0]);

						cout << "Set fov to " << to_string(Input::fov) << ".\n";
					}
					else
					{
						Input::fov = 90;

						cout << "Error: FOV value " << lineVariables[0] << " is out of range or not a float! Resetting to default.\n";
					}
				}
				else if (name == "camera_nearClip")
				{
					if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
					{
						Input::nearClip = stof(lineVariables[0]);

						cout << "Set camera near clip to " << to_string(Input::nearClip) << ".\n";
					}
					else
					{
						Input::nearClip = 0.001f;

						cout << "Error: Camera near clip value " << lineVariables[0]
							<< " is out of range or not a float! Resetting to default.\n";
					}
				}
				else if (name == "camera_farClip")
				{
					if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
					{
						Input::farClip = stof(lineVariables[0]);

						cout << "Set camera far clip to " << to_string(Input::farClip) << ".\n";
					}
					else
					{
						Input::farClip = 100.0f;

						cout << "Error: Camera far clip value " << lineVariables[0]
							<< " is out of range or not a float! Resetting to default.\n";
					}
				}
				else if (name == "camera_position")
				{
					if (ConfigFileManager::IsValueInRange(name + "X", lineVariables[0])
						&& ConfigFileManager::IsValueInRange(name + "Y", lineVariables[1])
						&& ConfigFileManager::IsValueInRange(name + "Z", lineVariables[2]))
					{
						vec3 newPosition = vec3(
							stof(lineVariables[0]),
							stof(lineVariables[1]),
							stof(lineVariables[2]));
						Render::camera.SetCameraPosition(newPosition);

						cout << "Set camera position to to "
							<< to_string(Render::camera.GetCameraPosition().x) << ", "
							<< to_string(Render::camera.GetCameraPosition().y) << ", "
							<< to_string(Render::camera.GetCameraPosition().z) << ".\n";
					}
					else
					{
						Render::camera.SetCameraPosition(vec3(0));

						cout << "Error: X, Y or Z position for value " << lineVariables[0]
							<< " camera is out of range or not a float! Resetting to default.\n";
					}
				}
				else if (name == "camera_rotation")
				{
					if (ConfigFileManager::IsValueInRange(name + "X", lineVariables[0])
						&& ConfigFileManager::IsValueInRange(name + "Y", lineVariables[1])
						&& ConfigFileManager::IsValueInRange(name + "Z", lineVariables[2]))
					{
						Render::camera.SetCameraRotation(vec3(
							stof(lineVariables[0]),
							stof(lineVariables[1]),
							stof(lineVariables[2])));

						cout << "Set camera rotation to to "
							<< to_string(Render::camera.GetCameraRotation().x) << ", "
							<< to_string(Render::camera.GetCameraRotation().y) << ", "
							<< to_string(Render::camera.GetCameraRotation().z) << ".\n";
					}
					else
					{
						Render::camera.SetCameraRotation(vec3(0));

						cout << "Error: X, Y or Z rotation value " << lineVariables[0]
							<< " for camera is out of range or not a float! Resetting to default.\n";
					}
				}
			}
		}

		configFile.close();

		UpdateValues();

		cout << "Successfully loaded config file!\n";
	}

	void ConfigFileManager::SaveConfigFile()
	{
		if (exists(configFilePath)) remove(configFilePath);
		UpdateValues();
		CreateNewConfigFile();

		cout << "Successfully saved config file!\n";
	}

	void ConfigFileManager::CreateNewConfigFile()
	{
		ofstream configFile(configFilePath);

		if (!configFile)
		{
			string title = "Failed to create config file";
			string description = "Couldn't create config file at " + configFilePath + "!";
			Game::CreateErrorPopup(title.c_str(), description.c_str());
		}

		for (const auto& variable : values)
		{
			configFile << variable.GetName() << ": " << variable.GetValue() << "\n";
		}

		configFile.close();
	}

	bool ConfigFileManager::IsValueInRange(
		const string& name,
		const string& value)
	{
		ConfigFileValue::Type type{};

		float currentFloatValue{}, minFloatValue{}, maxFloatValue{};
		int currentIntValue{}, minIntValue{}, maxIntValue{};
		vec2 currentVec2Value{}, minVec2Value{}, maxVec2Value{};
		vec3 currentVec3Value{}, minVec3Value{}, maxVec3Value{};

		for (auto& configFileValue : values)
		{
			if (configFileValue.GetName() == name)
			{
				type = configFileValue.GetType();

				if (type == ConfigFileValue::Type::type_float)
				{
					currentFloatValue = stof(configFileValue.GetValue());
					minFloatValue = stof(configFileValue.GetMinValue());
					maxFloatValue = stof(configFileValue.GetMaxValue());
				}
				else if (type == ConfigFileValue::Type::type_int)
				{
					currentIntValue = stoi(configFileValue.GetValue());
					minIntValue = stoi(configFileValue.GetMinValue());
					maxIntValue = stoi(configFileValue.GetMaxValue());
				}
				else if (type == ConfigFileValue::Type::type_vec2)
				{
					vector<string> currentSplitValue = String::Split(configFileValue.GetValue(), ',');
					currentVec2Value = vec2(stof(currentSplitValue[0]), stof(currentSplitValue[1]));

					vector<string> minSplitValue = String::Split(configFileValue.GetMinValue(), ',');
					minVec2Value = vec2(stof(minSplitValue[0]), stof(minSplitValue[1]));

					vector<string> maxSplitValue = String::Split(configFileValue.GetMaxValue(), ',');
					maxVec2Value = vec2(stof(maxSplitValue[0]), stof(maxSplitValue[1]));
				}
				else if (type == ConfigFileValue::Type::type_vec3)
				{
					vector<string> currentSplitValue = String::Split(configFileValue.GetValue(), ',');
					currentVec3Value = vec3(stof(currentSplitValue[0]), stof(currentSplitValue[1]), stof(currentSplitValue[2]));

					vector<string> minSplitValue = String::Split(configFileValue.GetMinValue(), ',');
					minVec3Value = vec3(stof(minSplitValue[0]), stof(minSplitValue[1]), stof(minSplitValue[2]));

					vector<string> maxSplitValue = String::Split(configFileValue.GetMaxValue(), ',');
					maxVec3Value = vec3(stof(maxSplitValue[0]), stof(maxSplitValue[1]), stof(maxSplitValue[2]));
				}

				break;
			}
		}

		bool isCorrectType = false;
		vec2 vec2Value{};
		vec3 vec3Value{};
		switch (type)
		{
		case ConfigFileValue::Type::type_string:
			return true;
		case ConfigFileValue::Type::type_float:
			isCorrectType = String::CanConvertStringToFloat(value);
			break;
		case ConfigFileValue::Type::type_int:
			isCorrectType = String::CanConvertStringToInt(value);
			break;
		case ConfigFileValue::Type::type_vec2:
			if (String::ContainsString(value, ", "))
			{
				string newValue = String::StringReplace(value, ", ", ",");
				vector<string> splitVec2 = String::Split(newValue, ',');
				vec2Value = vec2(stof(splitVec2[0]), stof(splitVec2[1]));
			}
			else return false;
			break;
		case ConfigFileValue::Type::type_vec3:
			if (String::ContainsString(value, ", "))
			{
				string newValue = String::StringReplace(value, ", ", ",");
				vector<string> splitVec3 = String::Split(newValue, ',');
				vec3Value = vec3(stof(splitVec3[0]), stof(splitVec3[1]), stof(splitVec3[2]));
			}
			break;
		}

		if (!isCorrectType) return false;

		float floatValue = stof(value);
		int intValue = stoi(value);

		if (type == ConfigFileValue::Type::type_float)
		{
			return floatValue >= minFloatValue && floatValue <= maxFloatValue;
		}
		else if (type == ConfigFileValue::Type::type_int)
		{
			return intValue >= minIntValue && intValue <= maxIntValue;
		}
		else if (type == ConfigFileValue::Type::type_vec2)
		{
			return vec2Value.x >= minVec2Value.x
				&& vec2Value.y >= minVec2Value.y
				&& vec2Value.x <= maxVec2Value.x
				&& vec2Value.y <= maxVec2Value.y;
		}
		else if (type == ConfigFileValue::Type::type_vec3)
		{
			return vec3Value.x >= minVec3Value.x
				&& vec3Value.y >= minVec3Value.y
				&& vec3Value.z >= minVec3Value.z
				&& vec3Value.x <= maxVec3Value.x
				&& vec3Value.y <= maxVec3Value.y
				&& vec3Value.z <= maxVec3Value.z;
		}

		return false;
	}
}