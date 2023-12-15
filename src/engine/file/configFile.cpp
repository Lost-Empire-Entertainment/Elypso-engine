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

using namespace Graphics;

namespace File
{
	void ConfigFile::ProcessConfigFile(const string& fileName)
	{
		path fullPath = current_path();
		string fullPathString = fullPath.generic_string();

		filePath = fullPathString + "/" + fileName;

		//check if file exists
		if (exists(filePath))
		{
			string line;
			while (getline(inputFile, line))
			{
				cout << line << endl;
			}

			inputFile.close();
		}
	}

	void ConfigFile::SaveDataAtShutdown()
	{
		//check if file exists
		if (exists(filePath))
		{
			if (remove(filePath)) 
			{
				cout << "File output.txt has been successfully deleted." << endl;
			}
			else 
			{
				cerr << "Error deleting config.txt!" << endl;
				return;
			}
		}
		else
		{
			//open the file for writing
			ofstream configFile("config.txt");

			//check if the file is open
			if (!configFile.is_open())
			{
				cerr << "Error opening new config.txt!" << endl;
			}
			else
			{
				//write config data into the config file
				configFile << "resolution: " << Render::SCR_WIDTH << ", " << Render::SCR_HEIGHT << endl;
				configFile << "vsync: " << Render::useMonitorRefreshRate << endl;
				configFile << "fps: " << Render::fov << endl;
				configFile << "camera near clip: " << Render::nearClip << endl;
				configFile << "camera far clip: " << Render::farClip << endl;
				configFile << "camera position: " <<
					Render::cameraPos.x << ", " <<
					Render::cameraPos.y << ", " <<
					Render::cameraPos.z << endl;
				configFile << "camera rotation: " <<
					Render::camera.GetCameraRotation().x << ", " <<
					Render::camera.GetCameraRotation().y << ", " <<
					Render::camera.GetCameraRotation().z << endl;

				configFile.close();

				cout << "Sucessfully saved data to config.txt!" << endl;
			}
		}
	}
}