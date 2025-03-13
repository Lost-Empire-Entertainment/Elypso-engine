//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <map>
#include <vector>

namespace EngineFile
{
	using std::string;
	using std::map;
	using std::vector;

	class SceneFile
	{
	public:
		enum class SaveType
		{
			defaultSave,
			sceneSwitch,
			shutDown
		};

		static inline bool unsavedChanges;
		static inline string currentProjectPath;
		static inline string targetLevel;

		static inline map<string, string> skyboxTexturesMap;

		static void LoadScene(const string& scenePath);
		static void SaveScene(
			SaveType saveType = SaveType::defaultSave, 
			const string& targetLevel = "", 
			bool sendSuccessMessage = true);

		/// <summary>
		/// Search by key to check if a variable exists in the keys vector.
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		static string GetKey(const string& key);
		/// <summary>
		/// Gets a value by key from the scene value map.
		/// </summary>
		/// <param name="key">The name of the key in the map we need a value for</param>
		/// <param name="silent">Whether a response on fail should be printed or not</param>
		/// <returns></returns>
		static string GetValue(const string& key, bool silent = false);
		/// <summary>
		/// Assigns a value by key to scene value map.
		/// </summary>
		/// <param name="key">The name of the key in the map we need to set a value for</param>
		/// <param name="value">The value the key will be set to</param>
		static void SetValue(const string& key, const string& value);
	private:
		static inline string sceneFilePath;

		/// <summary>
		/// Temporary storage for all config settings until engine is saved.
		/// </summary>
		static inline vector<string> keys;
		static inline vector<string> values;

		/// <summary>
		/// Saves default data when new scene file is loaded
		/// </summary>
		static void SetDefaultData();

		/// <summary>
		/// Saves global physics data from Elypso library to current scene file
		/// </summary>
		static void SaveGlobalPhysicsData();
		/// <summary>
		/// Loads global physics data from the current scene file to Elypso Library
		/// </summary>
		static void LoadGlobalPhysicsData();

		/// <summary>
		/// Saves global graphics data to current scene file
		/// </summary>
		static void SaveGlobalGraphicsData();
		/// <summary>
		/// Loads global graphics data from current scene file
		/// </summary>
		static void LoadGlobalGraphicsData();

		/// <summary>
		/// Creates a brand new scene file with default values.
		/// </summary>
		static void CreateNewSceneFile(const string& newPath);
	};
}