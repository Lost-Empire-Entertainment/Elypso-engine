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

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using std::string;
using std::ofstream;
using std::vector;

namespace Core
{
	class ConsoleManager
	{
	public:
		enum class Caller : char
		{
			ENGINE,
			INPUT,
			SHADER,
			WINDOW_SETUP,
			WINDOW_LOOP,
			OPENGL,
			GLFW,
			GLAD,
			TEXTURE,
			IMGUI,
			SHUTDOWN
		};
		enum class Type : char
		{
			SUCCESS,
			INFO,
			DEBUG,
			CLEANUP,
			EXCEPTION
		};

		static inline bool sendDebugMessages;

		/// <summary>
		/// A simple method that prints a message with a message type and timestamp to the console.
		/// </summary>
		/// <param name="caller">What called this message?</param>
		/// <param name="type">What type of a message is this?</param>
		/// <param name="message">The actual message.</param>
		/// <param name="onlyMessage">Should we ignore the timestamp and message type and only print the message?</param>
		static void WriteConsoleMessage(Caller caller, Type type, const string& message, bool onlyMessage = false);

		static void ParseConsoleCommand(const string& command);

		static void AddLog(const std::string& message);

		static void PrintLogsToBuffer();

		static vector<string> storedLogs;
	private:
		static inline bool wireframeMode;
	};

	class Timestamp
	{
	public:
		static string GetCurrentTimestamp();
	};

	class Logger
	{
	public:
		static void InitializeLogger();
		static void CloseLogger();
		static void AddLog(const string& message);
	private:
		static ofstream logFile;
	};
}