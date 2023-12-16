//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//external
#include "glm.hpp"

//engine
#include "configFile.hpp"
#include "render.hpp"
#include "console.hpp"
#include "stringUtils.hpp"
#include "searchUtils.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

using glm::vec3;
using std::cout;
using std::endl;
using std::to_string;
using std::ifstream;
using std::filesystem::exists;
using std::filesystem::remove;

using Graphics::Render;
using Core::ConsoleManager;
using Utils::String;
using Utils::Search;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace File
{
	void ConfigFile::ProcessConfigFile(const string& fileName)
	{
		filePath = Search::FindDocumentsFolder();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::DEBUG,
			"Game documents path: " + filePath + "\n");

		//check if file exists
		if (exists(filePath + "/config.txt"))
		{
			ifstream configFile(filePath + "/config.txt");
			if (!configFile.is_open())
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Couldn't open config.txt!\n\n");
				return;
			}
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::INFO,
				"Reading from config.txt...\n");

			string line;
			while (getline(configFile, line))
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

				//output config file results
				/*
				cout << "Name: " << name << endl;
				for (const auto& variable : lineVariables)
				{
					cout << "  - " << variable << endl;
				}
				*/
				if (name == "resolution")
				{
					if (ConfigFile::IsValueInRange("width", lineVariables[0])
						&& ConfigFile::IsValueInRange("height", lineVariables[1]))
					{
						unsigned int width = stoul(lineVariables[0]);
						Render::SCR_WIDTH = width;
						unsigned int height = stoul(lineVariables[1]);
						Render::SCR_HEIGHT = height;
						glfwSetWindowSize(Render::window, width, height);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set resolution to " + 
							to_string(Render::SCR_WIDTH) + ", " + 
							to_string(Render::SCR_HEIGHT) + ".\n");
					}
					else
					{
						glfwSetWindowSize(Render::window, 1280, 720);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Height or width for resolution is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "vsync")
				{
					if (ConfigFile::IsValueInRange("vsync", lineVariables[0]))
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
							"VSync is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "fov")
				{
					if (ConfigFile::IsValueInRange("fov", lineVariables[0]))
					{
						Render::fov = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set fov to " + to_string(Render::fov) + ".\n");
					}
					else
					{
						Render::fov = 90;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"FOV is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camnearclip")
				{
					if (ConfigFile::IsValueInRange("camnearclip", lineVariables[0]))
					{
						Render::nearClip = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera near clip to " + to_string(Render::nearClip) + ".\n");
					}
					else
					{
						Render::nearClip = 0.001f;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Camera near clip is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camfarclip")
				{
					if (ConfigFile::IsValueInRange("camfarclip", lineVariables[0]))
					{
						Render::farClip = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera far clip to " + to_string(Render::farClip) + ".\n");
					}
					else
					{
						Render::farClip = 100.0f;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Camera far clip is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "campos")
				{
					if (ConfigFile::IsValueInRange("camposx", lineVariables[0])
						&& ConfigFile::IsValueInRange("camposy", lineVariables[1])
						&& ConfigFile::IsValueInRange("camposz", lineVariables[2]))
					{
						Render::cameraPos.x = stof(lineVariables[0]);
						Render::cameraPos.y = stof(lineVariables[0]);
						Render::cameraPos.z = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera position to to " + 
							to_string(Render::cameraPos.x) + ", " + 
							to_string(Render::cameraPos.y) + ", " +
							to_string(Render::cameraPos.z) + ".\n");
					}
					else
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"X, Y or Z position for camera is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camrot")
				{
					if (ConfigFile::IsValueInRange("camrotx", lineVariables[0])
						&& ConfigFile::IsValueInRange("camroty", lineVariables[1])
						&& ConfigFile::IsValueInRange("camrotz", lineVariables[2]))
					{
						Render::camera.SetCameraRotation(vec3(
							stof(lineVariables[0]),
							stof(lineVariables[1]),
							stof(lineVariables[2])));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera rotation to to " +
							to_string(Render::camera.GetCameraRotation().x) + ", " +
							to_string(Render::camera.GetCameraRotation().y) + ", " +
							to_string(Render::camera.GetCameraRotation().z) + ".\n");
					}
					else
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"X, Y or Z rotation for camera is out of range or not a float! Resetting to default.\n");
					}
				}
				else
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						name + " is not a valid config file variable type!\n");
				}
			}

			configFile.close();
		}
	}

	void ConfigFile::SaveDataAtShutdown()
	{
		if (exists(filePath + "/config.txt"))
		{
			if (!remove(filePath + "/config.txt"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Couldn't delete config.txt!\n\n");
				return;
			}

			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::CLEANUP,
				"Deleted file: config.txt\n");
		}

		//open the file for writing
		ofstream configFile(filePath + "/config.txt");

		if (!configFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't open new config.txt!\n\n");
			return;
		}

		//write config data into the config file
		int width;
		int height;
		glfwGetWindowSize(Render::window, &width, &height);
		configFile << "resolution: " << width << ", " << height << endl;

		configFile << "vsync: " << Render::useMonitorRefreshRate << endl;
		configFile << "fov: " << Render::fov << endl;
		configFile << "camnearclip: " << Render::nearClip << endl;
		configFile << "camfarclip: " << Render::farClip << endl;
		configFile << "campos: " <<
			Render::cameraPos.x << ", " <<
			Render::cameraPos.y << ", " <<
			Render::cameraPos.z << endl;
		configFile << "camrot: " <<
			Render::camera.GetCameraRotation().x << ", " <<
			Render::camera.GetCameraRotation().y << ", " <<
			Render::camera.GetCameraRotation().z << endl;

		configFile.close();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::DEBUG,
			"Sucessfully saved data to config.txt!\n");
	}

	bool ConfigFile::IsValueInRange(string type, string value)
	{
		if (type == "width")
		{
			float width = stof(value);
			return (String::CanConvertStringToInt(value)
					&& width >= 1280
					&& width <= 7680);
		}
		else if (type == "height")
		{
			float height = stof(value);
			return (String::CanConvertStringToInt(value)
					&& height >= 720
					&& height <= 4320);
		}
		else if (type == "vsync")
		{
			int vsync = stoi(value);
			return (String::CanConvertStringToInt(value)
					&& (vsync == 0 
					|| vsync == 1));
		}
		else if (type == "fov")
		{
			float fov = stof(value);
			return (String::CanConvertStringToFloat(value)
					&& fov >= 70.0f
					&& fov <= 110.0f);
		}
		else if (type == "camnearclip")
		{
			float camnearclip = stof(value);
			return (String::CanConvertStringToFloat(value)
				&& camnearclip >= 0.001f
				&& camnearclip <= 10.0f);
		}
		else if (type == "camfarclip")
		{
			float camfarclip = stof(value);
			return (String::CanConvertStringToFloat(value)
				&& camfarclip >= 10.0f
				&& camfarclip <= 100.0f);
		}
		else if (type == "camposx"
				 || type == "camposy"
			     || type == "camposz"
				 || type == "camrotx"
				 || type == "camroty"
				 || type == "camrotz")
		{
			float val = stof(value);
			return (String::CanConvertStringToFloat(value)
				&& val >= -1000000.0f
				&& val <= 1000000.0f);
		}
		else
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				type + " is not a valid config file variable type!\n");
			return false;
		}
	}
}