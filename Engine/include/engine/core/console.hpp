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
			INFO,
			DEBUG,
			EXCEPTION
		};

		static inline bool sendDebugMessages;

		static vector<string> storedLogs;

		/// <summary>
		/// Print selected message to in-engine console.
		/// </summary>
		/// <param name="caller">What called the message.</param>
		/// <param name="type">What kind of a message is it.</param>
		/// <param name="message">The message itself.</param>
		/// <param name="onlyMessage">Do we only send the message without message caller, type and timestamp?</param>
		/// <param name="internalMessage">Do we also print this message to the in-engine console?</param>
		static void WriteConsoleMessage(Caller caller, Type type, const string& message, bool onlyMessage = false, bool internalMessage = true);

		static void ParseConsoleCommand(const string& command);

		static void AddLog(const std::string& message);

		static void PrintLogsToBuffer();
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