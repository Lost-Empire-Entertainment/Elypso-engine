//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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