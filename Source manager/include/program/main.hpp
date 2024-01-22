//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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

#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::filesystem::path;

namespace Core
{
	class Render
	{
	public:
		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void GUISetup();

		static void WindowLoop();

		static void Window_Main();
		static void Window_Console();

		static void Shutdown();
	private:
		static inline bool isCleanVSConfirmOpen;
		static inline bool isCleanEngineConfirmOpen;

		static inline unsigned int width = 1280;
		static inline unsigned int height = 720;
		static inline string programName = "Elypso engine source manager v2.0.1";
		static inline GLFWwindow* window;
		static inline float fontScale = 1.5f;

		static inline vector<string> consoleMessages;
		static inline const int maxConsoleMessages = 1000;

		static void RunBatFile(string command);

		static void MainWindow_Reconfigure_CMake();
		static void MainWindow_BuildEngine();
		static void MainWindow_CleanVS();
		static void MainWindow_CleanEngine();

		static void MainWindow_CleanVS_Confirm();
		static void MainWindow_CleanEngine_Confirm();

		static void ConsoleWindow_WriteToConsole(const string& message, bool printToConsole = false);
	};
}