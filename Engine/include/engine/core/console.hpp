//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

namespace Core
{
	using std::string;
	using std::vector;

	class ConsoleManager
	{
	public:
		enum Caller
		{
			FILE,       //Message originates from a file interaction
			INPUT,      //Message originates from a user input interaction
			INITIALIZE, //Message originates from an initializing function
			SHUTDOWN    //Message originates from the shutdown function
		};

		enum Type
		{
			INFO,     //Standard message type
			DEBUG,    //Message type that should only be displayed for debugging purposes
			EXCEPTION //Message type that should only be displayed when something is not working as intended
		};

		static inline bool sendDebugMessages;

		static inline vector<string> storedLogs;

		static string GetCurrentTimestamp();

		static void AddConsoleLog(const string& message);

		static void AddLoggerLog(const string& message);

		static void InitializeLogger();

		static void PrintLogsToBuffer();

		static void CloseLogger();

		static void ParseConsoleCommand(const string& message);

		/// <summary>
		/// Print selected message to in-game console.
		/// </summary>
		/// <param name="caller">What called the message.</param>
		/// <param name="type">What kind of a message is it.</param>
		/// <param name="message">The message itself.</param>
		/// <param name="onlyMessage">Do we only send the message without message caller, type and timestamp?</param>
		/// <param name="internalMessage">Do we also print this message to the in-game console?</param>
		static void WriteConsoleMessage(Caller caller, Type type, const string& message, bool onlyMessage = false, bool internalMessage = true);

	private:
		static inline bool wireframeMode;
	};
}