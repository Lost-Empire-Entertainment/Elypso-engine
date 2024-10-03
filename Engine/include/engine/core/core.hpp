//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace Core
{
	using std::string;

	class Engine
	{
	public:
		static inline string name;
		static inline string version;
		static inline string gameFirstScene;

		static inline string docsPath;
		static inline string filesPath;
		static inline string projectPath;

		static inline string scenePath;
		static inline string currentGameobjectsPath;
		static inline string scenesPath;
		static inline string texturesPath;
#if ENGINE_MODE
		static inline string gamePath;
		static inline string gameExePath;
		static inline string gameParentPath;
#endif
		static inline bool isEngineRunning;
		static inline bool startedWindowLoop;
#if ENGINE_MODE
		static void InitializeEngine(const string& version = "0.1.1");

		static void CheckForMissingCompilerFiles();
#else
		static void InitializeEngine(const string& version = "1.0");
#endif
		static void RunEngine();

#if DISCORD_MODE
		/// <summary>
		/// Initializes Discord presence with a specific appID
		/// </summary>
		/// <param name="appID">The app ID found in your app in Discord Developer Portal</param>
		static void InitializeDiscordRichPresence(const __int64& appID);
		/// <summary>
		/// Discord app ID runtime loop
		/// </summary>
		static void RunDiscordRichPresence();
		/// <summary>
		/// Displays this acttivity data in your profile in Discord
		/// </summary>
		/// <param name="state">Top title</param>
		/// <param name="details">Description</param>
		/// <param name="time_start">When the activity started, set to 0 to keep counting since this program started</param>
		/// <param name="time_end">When the activity will end, set to 0 to disable end time</param>
		/// <param name="largeImage">Big image to the left of the activity text</param>
		/// <param name="largeText">Tooltip when hovering over large image</param>
		/// <param name="smallImage">Small image on the bottom right corner of the large image</param>
		/// <param name="smallText">Tooltip when hovering over small image</param>
		static void SetDiscordRichPresence(
			const string& state,
			const string& details,
			const time_t& time_start = 0,
			const time_t& time_end = 0,
			const string& largeImage = "",
			const string& largeText = "",
			const string& smallImage = "",
			const string& smallText = "");
#endif

		static void CreateErrorPopup(const char* errorMessage = "MISSING ERROR MESSAGE");

		static bool IsThisProcessAlreadyRunning(const string& processName);

		/// <param name="immediate">Does the engine need to be shut down immediately?</param>
		static void Shutdown(bool immediate = false);
	};
}