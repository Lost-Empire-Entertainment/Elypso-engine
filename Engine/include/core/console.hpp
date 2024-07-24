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

using std::vector;

namespace Core
{
	class ConsoleManager
	{
	public:
		enum class Caller : char
		{
			//FILE
			FILE,
			SHADER,
			TEXTURE,

			//ENGINE
			INPUT,
			INITIALIZE,
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

		static void InitializeLogger();
		static void CloseLogger();

		/// <summary>
		/// Prints the messages to the console directly after console has initialized.
		/// </summary>
		/// <param name="message"></param>
		static void AddConsoleLog(const string& message);

		/// <summary>
		/// Print selected message to in-engine console.
		/// </summary>
		/// <param name="caller">What called the message.</param>
		/// <param name="type">What kind of a message is it.</param>
		/// <param name="message">The message itself.</param>
		/// <param name="onlyMessage">Do we only send the message without message caller, type and timestamp?</param>
		/// <param name="internalMessage">Do we also print this message to the in-engine console?</param>
		static void WriteConsoleMessage(Caller caller, Type type, const string& message, bool onlyMessage = false, bool internalMessage = true);

		/// <summary>
		/// Run inserted console command.
		/// </summary>
		/// <param name="command">user inserted command</param>
		static void ParseConsoleCommand(const string& command);

		/// <summary>
		/// Prints the logger messages to the console after console has initialized.
		/// </summary>
		static void PrintLogsToBuffer();

		/// <summary>
		/// Store console messages separately before console has initialized.
		/// </summary>
		/// <param name="message"></param>
		static void AddLoggerLog(const std::string& message);
	private:
		static inline string GetCurrentTimestamp();

		static inline bool wireframeMode;
	};
}