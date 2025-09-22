//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace Core
{
	using std::string;
	using std::vector;
	using std::find;

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
		static inline string gameRootFolder;
		static inline string gameExePath;
#endif
		/// <summary>
		/// Check if the engine or game is currently running.
		/// </summary>
		static inline bool isRunning;

		/// <summary>
		/// Initialize engine or game.
		/// </summary>
		static void InitializeEngine();
#if ENGINE_MODE
		/// <summary>
		/// Checks if all necessary files required for compiling the game inside the engine have been set up correctly.
		/// </summary>
		/// <returns></returns>
		static bool CheckForMissingCompilerFiles();
#endif

		/// <summary>
		/// Show an error popup that shuts down the engine or game immediately after the Ok button is pressed.
		/// </summary>
		static void CreateErrorPopup(const char* errorMessage);
		/// <summary>
		/// Show a warning popup that doesn't shut down the engine or game.
		/// </summary>
		static void CreateWarningPopup(const char* warningMessage);
		/// <summary>
		/// Checks whether the engine or game with the same name is already running or not.
		/// </summary>
		static bool IsThisProcessAlreadyRunning(const string& processName);

		/// <summary>
		/// Checks whether the engine or game is minimized. 
		/// </summary>
		static bool IsUserIdle();

		/// <summary>
		/// Checks whether the chosen name can be assigned to the name of a gameobject.
		/// </summary>
		static bool IsReservedName(const string& name) 
		{
			return find(
				reservedNames.begin(),
				reservedNames.end(),
				name) != reservedNames.end();
		}

		/// <summary>
		/// Engine and game runtime loop.
		/// </summary>
		static void RunEngine();

		/// <summary>
		/// Shut down the engine or game.
		/// </summary>
		/// <param name="immediate">Quick shutdown with fewer checks.</param>
		/// <param name="bypassSaveCheck">Shut down without checking if anything needs to be saved.</param>
		static void Shutdown(bool immediate = false, bool bypassSaveCheck = false);
	private:
		/// <summary>
		/// A vector of used engine gameobject names 
		/// that cannot be assigned to any names of 
		/// any gameobject types to prevent potential naming conflicts.
		/// </summary>
		static inline vector<string> reservedNames
		{
			"SceneCamera",          //engine-only camera gameobject
			"SelectedObjectAction", //selected object action gameobject
			"SelectedObjectBorder", //selected object border gameobject
			"Skybox"                //skybox gameobject
		};
	};
}
