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

//engine
#include "configFile.hpp"
#include "render.hpp"
#include "console.hpp"

using Graphics::Render;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace File
{
	void ConfigFile::ProcessConfigFile(const string& fileName)
	{
		path fullPath = current_path();
		string fullPathString = fullPath.generic_string();

		filePath = fullPathString + "/" + fileName;

		if (exists(filePath))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::DEBUG,
				"Config file path: " + filePath + "\n");

			ifstream configFile(filePath);
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
					unsigned int width = stoul(lineVariables[0]);
					Render::SCR_WIDTH = width;
					unsigned int height = stoul(lineVariables[1]);
					Render::SCR_HEIGHT = height;
					glfwSetWindowSize(Render::window, width, height);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set resolution to " + to_string(Render::SCR_WIDTH) + ", " + to_string(Render::SCR_HEIGHT) + "\n");
				}
			}
		}
	}

	void ConfigFile::SaveDataAtShutdown()
	{
		//check if file exists
		if (exists(filePath))
		{
			if (!remove(filePath)) 
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
		else
		{
			//open the file for writing
			ofstream configFile("config.txt");

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
	}
}