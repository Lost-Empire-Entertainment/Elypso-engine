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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

//external
#include "glm.hpp"

//engine
#include "configFile.hpp"
#include "render.hpp"
#include "console.hpp"
#include "stringUtils.hpp"
#include "searchUtils.hpp"
#include "gui.hpp"
#include "core.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

using glm::vec3;
using std::cout;
using std::endl;
using std::cerr;
using std::to_string;
using std::ifstream;
using std::exception;
using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::current_path;
using std::filesystem::create_directory;

using Core::Engine;
using Graphics::Render;
using Core::ConsoleManager;
using Utils::String;
using Utils::Search;
using Graphics::GUI;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace File
{
	void ConfigFile::SetConfigValuesToDefaultValues()
	{
		GUI::fontScale = 1.5f;
		//Render::SCR_WIDTH = 1280; //do not uncomment! edit in render.hpp instead!
		//Render::SCR_WIDTH = 720; //do not uncomment! edit in render.hpp instead!
		Render::useMonitorRefreshRate = true;
		Render::fov = 90.0f;
		Render::nearClip = 0.001f;
		Render::farClip = 100.0f;
		vec3 newPosition = vec3(0.0f, 0.0f, 3.0f);
		Render::camera.SetCameraPosition(newPosition);
		//Render::camera.SetCameraRotation(vec3(-90.0f, 0.0f, 0.0f)); //editing this has no effect because camera is initialized later
		GUI::allowScrollToBottom = true;
		ConsoleManager::sendDebugMessages = true;
		GUI::showKeybindsMenu = false;
		GUI::showDebugMenu = false;
		GUI::showConsole = false;

		Engine::docsPath = Search::FindDocumentsFolder();
		Engine::filesPath = current_path().generic_string() + "/files";

		if (!exists(Engine::docsPath))
		{
			try
			{
				create_directory(Engine::docsPath);
			}
			catch (const exception& e)
			{
				cerr << "Error creating path: " << e.what() << endl;
			}
		}
	}

	void ConfigFile::ProcessFirstConfigValues()
	{
		string configFilePath = Search::FindDocumentsFolder() + "/config.txt";
		string debugMessagesCheck = "consoleDebugMessages";
		string fontScale = "fontScale";

		SetConfigValuesToDefaultValues();

		if (!exists(configFilePath))
		{
			return;
		}

		ifstream configFileStream(configFilePath);

		if (!configFileStream.is_open())
		{
			cout << "Error opening config file at " << configFilePath << endl;
			return;
		}

		string line;
		while (getline(configFileStream, line))
		{
			vector<string> splitLine = String::Split(line, ' ');
			string cleanedVariable = String::Replace(splitLine[0], ":", "");
			string cleanedValue = splitLine[1];
			if (cleanedVariable == debugMessagesCheck
				&& String::CanConvertStringToInt(cleanedValue)
				&& (stoi(cleanedValue) == 0
				|| stoi(cleanedValue) == 1))
			{
				ConsoleManager::sendDebugMessages = stoi(cleanedValue) == 1;
			}
			else if (cleanedVariable == fontScale
					 && String::CanConvertStringToFloat(cleanedValue) 
					 && stof(cleanedValue) >= 1.0f
					 && stof(cleanedValue) <= 2.0f)
			{
				GUI::fontScale = stof(cleanedValue);
			}
		}

		configFileStream.close();
	}

	void ConfigFile::ProcessConfigFile(const string& fileName)
	{
		//check if file exists
		if (exists(Engine::docsPath + "/config.txt"))
		{
			ifstream configFile(Engine::docsPath + "/config.txt");
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
				Type::DEBUG,
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

				if (name == "fontScale")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
					{
						GUI::fontScale = stof(lineVariables[0]);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set font scale to " + to_string(GUI::fontScale) + ".\n");
					}
					else
					{
						GUI::fontScale = 1.5f;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Font scale value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "resolution")
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
							"Height or width value " + lineVariables[0] + " for resolution is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "vsync")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
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
				else if (name == "fov")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
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
							"FOV value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camNearClip")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
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
							"Camera near clip value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camFarClip")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
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
							"Camera far clip value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camPos")
				{
					if (ConfigFile::IsValueInRange(name + "X", lineVariables[0])
						&& ConfigFile::IsValueInRange(name + "Y", lineVariables[1])
						&& ConfigFile::IsValueInRange(name + "Z", lineVariables[2]))
					{
						vec3 newPosition = vec3(
							stof(lineVariables[0]),
							stof(lineVariables[1]),
							stof(lineVariables[2]));
						Render::camera.SetCameraPosition(newPosition);

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set camera position to to " + 
							to_string(Render::camera.GetCameraPosition().x) + ", " + 
							to_string(Render::camera.GetCameraPosition().y) + ", " +
							to_string(Render::camera.GetCameraPosition().z) + ".\n");
					}
					else
					{
						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"X, Y or Z position for value " + lineVariables[0] + " camera is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "camRot")
				{
					if (ConfigFile::IsValueInRange(name + "X", lineVariables[0])
						&& ConfigFile::IsValueInRange(name + "Y", lineVariables[1])
						&& ConfigFile::IsValueInRange(name + "Z", lineVariables[2]))
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
							"X, Y or Z rotation value " + lineVariables[0] + " for camera is out of range or not a float! Resetting to default.\n");
					}
				}
				else if (name == "consoleForceScroll")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
					{
						GUI::allowScrollToBottom = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set console force scroll to " + to_string(GUI::allowScrollToBottom) + ".\n");
					}
					else
					{
						GUI::allowScrollToBottom = true;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Console force scroll value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "showKeybindsMenu")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
					{
						GUI::showKeybindsMenu = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set show keybinds menu to " + to_string(GUI::showKeybindsMenu) + ".\n");
					}
					else
					{
						GUI::showKeybindsMenu = false;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Show keybinds menu value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "showDebugMenu")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
					{
						GUI::showDebugMenu = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set show debug menu to " + to_string(GUI::showDebugMenu) + ".\n");
					}
					else
					{
						GUI::showDebugMenu = true;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Show debug menu value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "showConsole")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
					{
						GUI::showConsole = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set show console to " + to_string(GUI::showConsole) + ".\n");
					}
					else
					{
						GUI::showConsole = true;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Show console value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
				else if (name == "showSceneMenu")
				{
					if (ConfigFile::IsValueInRange(name, lineVariables[0]))
					{
						GUI::showSceneMenu = static_cast<bool>(stoi(lineVariables[0]));

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::DEBUG,
							"Set show scene menu to " + to_string(GUI::showSceneMenu) + ".\n");
					}
					else
					{
						GUI::showSceneMenu = true;

						ConsoleManager::WriteConsoleMessage(
							Caller::ENGINE,
							Type::EXCEPTION,
							"Show scene menu value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
					}
				}
			}

			configFile.close();
		}
	}

	void ConfigFile::SaveDataAtShutdown()
	{
		if (exists(Engine::docsPath + "/config.txt"))
		{
			if (!remove(Engine::docsPath + "/config.txt"))
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
		ofstream configFile(Engine::docsPath + "/config.txt");

		if (!configFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't open new config.txt!\n\n");
			return;
		}

		//write config data into the config file
		configFile << "fontScale: " << GUI::fontScale << endl;
		int width;
		int height;
		glfwGetWindowSize(Render::window, &width, &height);
		configFile << "resolution: " << width << ", " << height << endl;
		configFile << "vsync: " << Render::useMonitorRefreshRate << endl;
		configFile << "fov: " << Render::fov << endl;
		configFile << "camNearClip: " << Render::nearClip << endl;
		configFile << "camFarClip: " << Render::farClip << endl;
		configFile << "camPos: " <<
			Render::camera.GetCameraPosition().x << ", " <<
			Render::camera.GetCameraPosition().y << ", " <<
			Render::camera.GetCameraPosition().z << endl;
		configFile << "camRot: " <<
			Render::camera.GetCameraRotation().x << ", " <<
			Render::camera.GetCameraRotation().y << ", " <<
			Render::camera.GetCameraRotation().z << endl;
		configFile << "consoleForceScroll: " << GUI::allowScrollToBottom << endl;
		configFile << "consoleDebugMessages: " << ConsoleManager::sendDebugMessages << endl;
		configFile << "showKeybindsMenu: " << GUI::showKeybindsMenu << endl;
		configFile << "showDebugMenu: " << GUI::showDebugMenu << endl;
		configFile << "showConsole: " << GUI::showConsole << endl;
		configFile << "showSceneMenu: " << GUI::showSceneMenu << endl;

		configFile.close();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::DEBUG,
			"Sucessfully saved data to config.txt!\n");
	}

	bool ConfigFile::IsValueInRange(string type, string value)
	{
		try
		{
			if (type == "fontScale")
			{
				float fontScale = stof(value);
				return (String::CanConvertStringToFloat(value)
						&& fontScale >= 1.0f
						&& fontScale <= 2.0f);
			}
			else if (type == "width")
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
			else if (type == "camNearClip")
			{
				float camnearclip = stof(value);
				return (String::CanConvertStringToFloat(value)
						&& camnearclip >= 0.001f
						&& camnearclip <= 10.0f);
			}
			else if (type == "camFarClip")
			{
				float camfarclip = stof(value);
				return (String::CanConvertStringToFloat(value)
						&& camfarclip >= 10.0f
						&& camfarclip <= 100.0f);
			}
			else if (type == "camPosX"
					 || type == "camPosY"
					 || type == "camPosZ")
			{
				float val = stof(value);
				return (String::CanConvertStringToFloat(value)
						&& val >= -1000000.0f
						&& val <= 1000000.0f);
			}
			else if (type == "camRotX"
					 || type == "camRotY"
					 || type == "camRotZ")
			{
				float val = stof(value);
				return (String::CanConvertStringToFloat(value)
					&& val >= -360.0f
					&& val <= 360.0f);
			}
			else if (type == "consoleForceScroll")
			{
				int consoleForceScroll = stoi(value);
				return (String::CanConvertStringToInt(value)
						&& (consoleForceScroll == 0
						|| consoleForceScroll == 1));
			}
			else if (type == "showKeybindsMenu")
			{
				int showKeybindsMenu = stoi(value);
				return (String::CanConvertStringToInt(value)
					&& (showKeybindsMenu == 0
						|| showKeybindsMenu == 1));
			}
			else if (type == "showDebugMenu")
			{
				int showDebugMenu = stoi(value);
				return (String::CanConvertStringToInt(value)
					&& (showDebugMenu == 0
						|| showDebugMenu == 1));
			}
			else if (type == "showConsole")
			{
				int showConsole = stoi(value);
				return (String::CanConvertStringToInt(value)
					&& (showConsole == 0
						|| showConsole == 1));
			}
			else if (type == "showSceneMenu")
			{
				int showSceneMenu = stoi(value);
				return (String::CanConvertStringToInt(value)
					&& (showSceneMenu == 0
						|| showSceneMenu == 1));
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
		catch (const exception& e)
		{
			return false;
		}
	}
}