//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>
#include <filesystem>

//external
#include "glm.hpp"
#include "glfw3.h"

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
using glm::vec3;
using std::ios;

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
			//opens file in binary mode and moves to end
			//if file is empty then a new one is created
			ifstream configFileBinary(configFilePath, ios::binary | ios::ate); 
			if (configFileBinary.tellg() == 0)
			{
				configFileBinary.close();

				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::INFO,
					"Failed to load config file '" + configFilePath + "' because it was empty! Creating a new one.\n\n");

				CreateNewConfigFile();
				return;
			}

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

					if (type == "camera_position")
					{
						vector<string> posSplit = String::Split(value, ',');
						vec3 pos = vec3(
							stof(posSplit[0]), 
							stof(posSplit[1]),
							stof(posSplit[2]));
						Render::camera.SetCameraPosition(pos);

						cout << "set cam pos to " 
							<< posSplit[0] << ", "  
							<< posSplit[1] << ", "
							<< posSplit[2]
							<< "\n";
					}
					else if (type == "camera_rotation")
					{
						vector<string> rotSplit = String::Split(value, ',');
						vec3 rot = vec3(
							stof(rotSplit[0]), 
							stof(rotSplit[1]),
							stof(rotSplit[2]));
						Render::camera.SetCameraRotation(rot);

						cout << "set cam rot to "
							<< rotSplit[0] << ", "
							<< rotSplit[1] << ", "
							<< rotSplit[2]
							<< "\n";
					}
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

		for (const auto& key : defaultConfigKeyOrder)
		{
			string value = configValues[key];

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

		if (SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(false);

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::INFO,
			"\nSuccessfully saved config file '" + configFilePath + "'!\n");
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

		//
		// CONFIG VALUES EDITABLE THROUGH ENGINE
		//

		configValues["gui_fontScale"] = "1.5";

		configValues["window_vsync"] = "1";

		configValues["camera_speedMultiplier"] = "1.0";
		configValues["camera_fov"] = "90.0";
		configValues["camera_nearClip"] = "0.001";
		configValues["camera_farClip"] = "100.0";
		configValues["camera_position"] = "0.0, 1.0, 0.0";
		configValues["camera_rotation"] = "-90.00, 0.00, 0.00";

		configValues["grid_color"] = "0.4, 0.4, 0.4";
		configValues["grid_transparency"] = "0.25";
		configValues["grid_maxDistance"] = "25.0";

		configValues["gui_inspector"] = "1";
		configValues["gui_console"] = "1";
		configValues["gui_sceneHierarchy"] = "1";
		configValues["gui_projectHierarchy"] = "1";
		configValues["gui_sceneMenu"] = "0";

		//separator
		configValues[".", "."];
		configValues["-", "-"];
		configValues[".", "."];

		//
		// GUI STYLE VALUES NOT EDITABLE THROUGH ENGINE
		//

		configValues["gui_Alpha"] = "1.0";
		configValues["gui_DisabledAlpha"] = "0.6";
		configValues["gui_WindowPadding"] = "8.0,8.0";
		configValues["gui_WindowRounding"] = "4.0";
		configValues["gui_WindowBorderSize"] = "1.0";
		configValues["gui_WindowMenuButtonPosition"] = "1";
		configValues["gui_ChildRounding"] = "0.0";
		configValues["gui_ChildBorderSize"] = "1.0";
		configValues["gui_PopupRounding"] = "4.0";
		configValues["gui_PopupBorderSize"] = "1.0";
		configValues["gui_FramePadding"] = "4.0,3.0";
		configValues["gui_FrameRounding"] = "2.5";
		configValues["gui_FrameBorderSize"] = "0.0";
		configValues["gui_ItemSpacing"] = "8.0,4.0";
		configValues["gui_ItemInnerSpacing"] = "4.0,4.0";
		configValues["gui_CellPadding"] = "4.0,2.0";
		configValues["gui_IndentSpacing"] = "21.0";
		configValues["gui_ColumnsMinSpacing"] = "6.0";
		configValues["gui_ScrollbarSize"] = "11.0";
		configValues["gui_ScrollbarRounding"] = "2.5";
		configValues["gui_GrabMinSize"] = "10.0";
		configValues["gui_GrabRounding"] = "2.0";
		configValues["gui_TabRounding"] = "3.5";
		configValues["gui_TabBorderSize"] = "0.0";
		configValues["gui_TabMinWidthForCloseButton"] = "0.0";
		configValues["gui_ColorButtonPosition"] = "1";
		configValues["gui_ButtonTextAlign"] = "0.5,0.5";
		configValues["gui_SelectableTextAlign"] = "0.0,0.0";

		//separator
		configValues[".", "."];
		configValues["-", "-"];
		configValues[".", "."];

		//
		// GUI COLOR VALUES NOT EDITABLE THROUGH ENGINE
		//

		configValues["gui_Color_Text"] = "1.0, 1.0, 1.0, 1.0";
		configValues["gui_Color_TextDisabled"] = "0.592156, 0.592156, 0.592156, 1.0";
		configValues["gui_Color_WindowBg"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_ChildBg"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_PopupBg"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_Border"] = "0.305882, 0.305882, 0.305882, 1.0";
		configValues["gui_Color_BorderShadow"] = "0.305882, 0.305882, 0.305882, 1.0";
		configValues["gui_Color_FrameBg"] = "0.200000, 0.200000, 0.215686, 1.0";
		configValues["gui_Color_FrameBgHovered"] = "0.113725, 0.592156, 0.925490, 1.0";
		configValues["gui_Color_FrameBgActive"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_TitleBg"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_TitleBgActive"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_TitleBgCollapsed"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_MenuBarBg"] = "0.200000, 0.200000, 0.215686, 1.0";
		configValues["gui_Color_ScrollbarBg"] = "0.200000, 0.200000, 0.215686, 1.0";
		configValues["gui_Color_ScrollbarGrab"] = "0.321568, 0.321568, 0.333333, 1.0";
		configValues["gui_Color_ScrollbarGrabHovered"] = "0.352941, 0.352941, 0.372549, 1.0";
		configValues["gui_Color_ScrollbarGrabActive"] = "0.352941, 0.352941, 0.372549, 1.0";
		configValues["gui_Color_CheckMark"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_SliderGrab"] = "0.113725, 0.592156, 0.925490, 1.0";
		configValues["gui_Color_SliderGrabActive"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_Button"] = "0.200000, 0.200000, 0.215686, 1.0";
		configValues["gui_Color_ButtonHovered"] = "0.113725, 0.592156, 0.925490, 1.0";
		configValues["gui_Color_ButtonActive"] = "0.113725, 0.592156, 0.925490, 1.0";
		configValues["gui_Color_Header"] = "0.200000, 0.200000, 0.215686, 1.0";
		configValues["gui_Color_HeaderHovered"] = "0.113725, 0.592156, 0.925490, 1.0";
		configValues["gui_Color_HeaderActive"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_Separator"] = "0.305882, 0.305882, 0.305882, 1.0";
		configValues["gui_Color_SeparatorHovered"] = "0.305882, 0.305882, 0.305882, 1.0";
		configValues["gui_Color_SeparatorActive"] = "0.305882, 0.305882, 0.305882, 1.0";
		configValues["gui_Color_ResizeGrip"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_ResizeGripHovered"] = "0.200000, 0.200000, 0.215686, 1.0";
		configValues["gui_Color_ResizeGripActive"] = "0.321568, 0.321568, 0.333333, 1.0";
		configValues["gui_Color_Tab"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_TabHovered"] = "0.113725, 0.592156, 0.925490, 1.0";
		configValues["gui_Color_TabActive"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_TabUnfocused"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_TabUnfocusedActive"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_PlotLines"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_PlotLinesHovered"] = "0.113725, 0.592156, 0.925490, 1.0";
		configValues["gui_Color_PlotHistogram"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_PlotHistogramHovered"] = "0.113725, 0.592156, 0.925490, 1.0";
		configValues["gui_Color_TableHeaderBg"] = "0.188235, 0.188235, 0.200000, 1.0";
		configValues["gui_Color_TableBorderStrong"] = "0.309803, 0.309803, 0.349019, 1.0";
		configValues["gui_Color_TableBorderLight"] = "0.227450, 0.227450, 0.247058, 1.0";
		configValues["gui_Color_TableRowBg"] = "0.0, 0.0, 0.0, 0.0";
		configValues["gui_Color_TableRowBgAlt"] = "1.0, 1.0, 1.0, 0.059999";
		configValues["gui_Color_TextSelectedBg"] = "0.0, 0.466666, 0.784313, 1.0";
		configValues["gui_Color_DragDropTarget"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_NavHighlight"] = "0.145098, 0.145098, 0.149019, 1.0";
		configValues["gui_Color_NavWindowingHighlight"] = "1.0, 1.0, 1.0, 0.699999";
		configValues["gui_Color_NavWindowingDimBg"] = "0.800000, 0.800000, 0.800000, 0.200000";
		configValues["gui_Color_ModalWindowDimBg"] = "0.145098, 0.145098, 0.149019, 1.0";

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

		defaultConfigKeyOrder.clear();
		for (const auto& kvp : configValues)
		{
			defaultConfigKeyOrder.push_back(kvp.first);
		}
		reverse(defaultConfigKeyOrder.begin(), defaultConfigKeyOrder.end());

		LoadConfigFile();
	}
}