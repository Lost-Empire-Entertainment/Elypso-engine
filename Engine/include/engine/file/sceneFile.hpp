//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <map>

namespace EngineFile
{
	using std::string;
	using std::map;

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
		static void SaveScene(SaveType saveType = SaveType::defaultSave, const string& targetLevel = "", bool sendSuccessMessage = true);
	};
}