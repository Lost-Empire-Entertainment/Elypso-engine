//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace Core
{
	using std::string;
	using std::vector;
	using std::unordered_map;
	using std::function;

	enum class CommandTarget
	{
		TARGET_ENGINE,
		TARGET_GAME,
		TARGET_BOTH
	};
	struct Command
	{
		string description{};
		unsigned int parameterCount{};
		CommandTarget target{};
		function<void(const vector<string>&)> action{};
	};

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

		static void AddCommands();

		static void InitializeLogger();

		static void PrintLogsToBuffer();

		static void CloseLogger();

		static void ParseConsoleCommand(const string& message);

		/// <summary>
		/// Assign a new command to the console.
		/// </summary>
		static void AddCommand(
			const string& command,
			const string& description,
			unsigned int parameterCount,
			CommandTarget target,
			function<void(const vector<string>&)> action);

		/// <summary>
		/// Print selected message to engine or game console.
		/// </summary>
		/// <param name="caller">What called the message.</param>
		/// <param name="type">What kind of a message is it.</param>
		/// <param name="message">The message itself.</param>
		/// <param name="onlyMessage">Do we only send the message without message caller, type and timestamp?</param>
		/// <param name="internalMessage">Do we also print this message to the engine or game console?</param>
		static void WriteConsoleMessage(
			Caller caller, 
			Type type, 
			const string& message, 
			bool onlyMessage = false, 
			bool internalMessage = true);

	private:
		static inline bool wireframeMode;

		static inline unordered_map<string, Command> engineCommands;
		static inline unordered_map<string, Command> gameCommands;

		static void AddConsoleLog(const string& message);

		static void AddLoggerLog(const string& message);

		static void FindCommand(
			const string& command,
			const vector<string>& parameters);

		//
		// ALL CONSOLE COMMANDS USED IN BOTH ENGINE AND GAME
		//

		/// <summary>
		/// Lists all engine or game commands.
		/// </summary>
		static void Command_Help_All(const vector<string>& args);
		/// <summary>
		/// Lists info about the selected command.
		/// </summary>
		static void Command_Help_Info(const vector<string>& args);
		/// <summary>
		/// Sets the OpenGL render mode. 1 = Shaded, 2 = Wireframe.
		/// </summary>
		static void Command_SetRenderMode(const vector<string>& args);
		/// <summary>
		/// Force-quits the engine or game, bypassing all checks and quickly saving the most important things.
		/// </summary>
		static void Command_Quit(const vector<string>& args);

		//
		// ALL CONSOLE COMMANDS IN ENGINE
		//

#if ENGINE_MODE
		/// <summary>
		/// Resets the scene camera position back to origin (0, 0, 0).
		/// </summary>
		static void Command_Engine_ResetCamera(const vector<string>& args);
#endif
	};
}