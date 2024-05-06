//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>
#include <algorithm>

//external
#include "glm.hpp"

//engine
#include "configFile.hpp"
#include "render.hpp"
#include "console.hpp"
#include "stringutils.hpp"
#include "gui.hpp"
#include "gui_console.hpp"
#include "gui_debugmenu.hpp"
#include "gui_inspector.hpp"
#include "gui_nodeblock.hpp"
#include "gui_assetlist.hpp"
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

using EngineFile::SceneFile;
using Core::Input;
using Core::Engine;
using Graphics::Render;
using Core::ConsoleManager;
using Utils::String;
using Graphics::GUI::EngineGUI;
using Graphics::GUI::GUIConsole;
using Graphics::GUI::GUIDebugMenu;
using Graphics::GUI::GUIInspector;
using Graphics::GUI::GUINodeBlock;
using Graphics::GUI::GUIAssetList;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace EngineFile
{
	ConfigFileManager configFileManager;

	void ConfigFileManager::SetDefaultConfigValues()
	{
		EngineGUI::fontScale = 1.5f;
		Render::windowWidth = 1280;
		Render::windowHeight = 720;
		Render::useMonitorRefreshRate = true;
		Input::moveSpeedMultiplier = 1.0f;
		Input::nearClip = 0.001f;
		Input::farClip = 100.0f;
		vec3 camPos = vec3(0.0f, 1.0f, 0.0f);
		Render::camera.SetCameraPosition(camPos);

		configFileManager.UpdateValues();
	}

	void ConfigFileManager::UpdateValues()
	{
		values.clear();

		ConfigFileValue gui_fontScale(
			"gui_fontScale",
			to_string(EngineGUI::fontScale),
			"1.0",
			"2.0",
			ConfigFileValue::Type::type_float);
		AddValue(gui_fontScale);

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

		ConfigFileValue gui_inspector(
			"gui_inspector",
			to_string(GUIInspector::renderInspector),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_inspector);

		ConfigFileValue gui_debugMenu(
			"gui_debugMenu",
			to_string(GUIDebugMenu::renderDebugMenu),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_debugMenu);

		ConfigFileValue gui_console(
			"gui_console",
			to_string(GUIConsole::renderConsole),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_console);

		ConfigFileValue gui_nodeBlockWindow(
			"gui_nodeBlockWindow",
			to_string(GUINodeBlock::renderNodeBlock),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_nodeBlockWindow);

		ConfigFileValue gui_assetListWindow(
			"gui_assetListWindow",
			to_string(GUIAssetList::renderAssetList),
			"0",
			"1",
			ConfigFileValue::Type::type_int);
		AddValue(gui_assetListWindow);
	}

	void ConfigFileManager::LoadConfigFile()
	{
		if (configFileManager.configFilePath == "")
		{
			configFileManager.configFilePath = Engine::docsPath + "/config.txt";

			if (!exists(configFileManager.configFilePath))
			{
				SetDefaultConfigValues();
				configFileManager.CreateNewConfigFile();
			}
		}

		ifstream configFile(configFileManager.configFilePath);

		if (!configFile)
		{
			string title = "Failed to open config file";
			string description = "Couldn't open config file at " + configFileManager.configFilePath + "!";
			Engine::CreateErrorPopup(title.c_str(), description.c_str());
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

				if (name == "gui_fontScale")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						EngineGUI::fontScale = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set font scale to " + to_string(EngineGUI::fontScale) + ".\n");
					}
					else
					{
						EngineGUI::fontScale = 1.5f;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Font scale value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "window_resolution")
				{
					if (configFileManager.IsValueInRange("width", lineVariables[0])
						&& configFileManager.IsValueInRange("height", lineVariables[1]))
					{
						unsigned int width = stoul(lineVariables[0]);
						Render::windowWidth = width;
						unsigned int height = stoul(lineVariables[1]);
						Render::windowHeight = height;
						glfwSetWindowSize(Render::window, width, height);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set resolution to "
							+ to_string(Render::windowWidth) + ", "
							+ to_string(Render::windowHeight) + ".\n");
					}
					else
					{
						glfwSetWindowSize(Render::window, 1280, 720);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Height or width value " + lineVariables[0]
							+ " for resolution is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "window_vsync")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						Render::useMonitorRefreshRate = static_cast<bool>(stoi(lineVariables[0]));
						glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set vsync to " + to_string(Render::useMonitorRefreshRate) + ".\n");
					}
					else
					{
						Render::useMonitorRefreshRate = true;
						glfwSwapInterval(1);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"VSync value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "camera_fov")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						Input::fov = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set fov to " + to_string(Input::fov) + ".\n");
					}
					else
					{
						Input::fov = 90;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"FOV value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camera_nearClip")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						Input::nearClip = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera near clip to " + to_string(Input::nearClip) + ".\n");
					}
					else
					{
						Input::nearClip = 0.001f;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Camera near clip value " + lineVariables[0]
							+ " is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camera_farClip")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						Input::farClip = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera far clip to " + to_string(Input::farClip) + ".\n");
					}
					else
					{
						Input::farClip = 100.0f;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Camera far clip value " + lineVariables[0]
							+ " is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camera_position")
				{
					if (configFileManager.IsValueInRange(name + "X", lineVariables[0])
						&& configFileManager.IsValueInRange(name + "Y", lineVariables[1])
						&& configFileManager.IsValueInRange(name + "Z", lineVariables[2]))
					{
						vec3 newPosition = vec3(
							stof(lineVariables[0]),
							stof(lineVariables[1]),
							stof(lineVariables[2]));
						Render::camera.SetCameraPosition(newPosition);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera position to to "
							+ to_string(Render::camera.GetCameraPosition().x) + ", "
							+ to_string(Render::camera.GetCameraPosition().y) + ", "
							+ to_string(Render::camera.GetCameraPosition().z) + ".\n");
					}
					else
					{
						Render::camera.SetCameraPosition(vec3(0));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"X, Y or Z position for value " + lineVariables[0]
							+ " camera is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camera_rotation")
				{
					if (configFileManager.IsValueInRange(name + "X", lineVariables[0])
						&& configFileManager.IsValueInRange(name + "Y", lineVariables[1])
						&& configFileManager.IsValueInRange(name + "Z", lineVariables[2]))
					{
						Render::camera.SetCameraRotation(vec3(
							stof(lineVariables[0]),
							stof(lineVariables[1]),
							stof(lineVariables[2])));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera rotation to to "
							+ to_string(Render::camera.GetCameraRotation().x) + ", "
							+ to_string(Render::camera.GetCameraRotation().y) + ", "
							+ to_string(Render::camera.GetCameraRotation().z) + ".\n");
					}
					else
					{
						Render::camera.SetCameraRotation(vec3(0));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"X, Y or Z rotation value " + lineVariables[0]
							+ " for camera is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "gui_inspector")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						GUIInspector::renderInspector = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set render inspector to " + to_string(GUIInspector::renderInspector) + ".\n");
					}
					else
					{
						GUIInspector::renderInspector = false;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Render inspector value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "gui_debugMenu")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						GUIDebugMenu::renderDebugMenu = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set render debug menu to " + to_string(GUIDebugMenu::renderDebugMenu) + ".\n");
					}
					else
					{
						GUIDebugMenu::renderDebugMenu = false;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Render debug menu value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "gui_console")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						GUIConsole::renderConsole = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set render console to " + to_string(GUIConsole::renderConsole) + ".\n");
					}
					else
					{
						GUIConsole::renderConsole = false;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Render console value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "gui_nodeBlockWindow")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						GUINodeBlock::renderNodeBlock = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set render node block window to " + to_string(GUINodeBlock::renderNodeBlock) + ".\n");
					}
					else
					{
						GUINodeBlock::renderNodeBlock = false;
						
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Render node block window value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "gui_assetListWindow")
				{
					if (configFileManager.IsValueInRange(name, lineVariables[0]))
					{
						GUIAssetList::renderAssetList = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set render asset list window to " + to_string(GUIAssetList::renderAssetList) + ".\n");
					}
					else
					{
						GUIAssetList::renderAssetList = false;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Render asset list window value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
			}
		}

		configFile.close();

		configFileManager.UpdateValues();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully loaded config file!\n");
	}

	void ConfigFileManager::SaveConfigFile()
	{
		if (exists(configFileManager.configFilePath))
		{
			remove(configFileManager.configFilePath);
		}
		configFileManager.UpdateValues();
		configFileManager.CreateNewConfigFile();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"Successfully saved config file!\n");
	}

	void ConfigFileManager::CreateNewConfigFile()
	{
		ofstream configFile(configFilePath);

		if (!configFile)
		{
			string title = "Failed to create config file";
			string description = "Couldn't create config file at " + configFilePath + "!";
			Engine::CreateErrorPopup(title.c_str(), description.c_str());
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