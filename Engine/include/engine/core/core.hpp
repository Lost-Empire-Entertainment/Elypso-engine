//Copyright(C) 2025 Lost Empire Entertainment
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
#if ENGINE_MODE
		static void InitializeEngine();

		static bool CheckForMissingCompilerFiles();
#else
		static void InitializeEngine();
#endif

		static void CreateErrorPopup(const char* errorMessage);
		static void CreateWarningPopup(const char* warningMessage);
		static bool IsThisProcessAlreadyRunning(const string& processName);

		static bool IsUserIdle();

		static void RunEngine();

		/// <summary>
		/// Shut down the engine or game.
		/// </summary>
		/// <param name="immediate">Quick shutdown with fewer checks.</param>
		/// <param name="bypassSaveCheck">Shut down without checking if anything needs to be saved.</param>
		static void Shutdown(bool immediate = false, bool bypassSaveCheck = false);
	};
}
