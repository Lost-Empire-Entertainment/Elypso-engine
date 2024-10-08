//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

using std::string;

namespace Core
{
	class Engine
	{
	public:
		static inline string name = "Elypso engine";
		static inline string version = "0.1.1";
		static inline string gameFirstScene;

		static inline string docsPath;
		static inline string filesPath;
		static inline string projectPath;

		static inline string scenePath;
		static inline string currentGameobjectsPath;
		static inline string scenesPath;
		static inline string texturesPath;

		static inline string gamePath;
		static inline string gameExePath;
		static inline string gameParentPath;

		static inline bool startedWindowLoop;

		static void InitializeEngine();
		static void RunEngine();

		static void CreateErrorPopup(
			const char* errorTitle = "MISSING ERROR TITLE", 
			const char* errorMessage = "MISSING ERROR MESSAGE");

		static bool IsThisProcessAlreadyRunning(const string& processName);

		/// <param name="immediate">Does the engine need to be shut down immediately?</param>
		static void Shutdown(bool immediate = false);
	};
}