//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <map>

using std::string;
using std::map;

namespace GameFile
{
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
		static inline string currentScenePath;
		static inline string targetLevel;

		static void CheckForGameFile();
		static void CreateScene();
		static void LoadScene(const string& levelName);
		static void LoadGameObject(const map<string, string> obj);
		static void SaveScene(SaveType saveType = SaveType::defaultSave, const string& targetLevel = "");
		/// <summary>
		/// Export all game files to game folder when shutting down.
		/// </summary>
		static void ExportGameFiles();
	};
}