//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <map>

using std::string;
using std::map;

namespace EngineFile
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
		static inline string targetLevel;

		static void CheckForProjectFile();
		static void CreateScene();
		static void LoadScene(const string& scenePath);
		static void LoadGameObject(const map<string, string> obj);
		static void SaveScene(SaveType saveType = SaveType::defaultSave, const string& targetLevel = "");
	private:
		static inline map<string, bool> models;
	};
}