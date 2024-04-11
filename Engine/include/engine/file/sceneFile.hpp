//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

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
		static inline string currentScenePath;
		static inline string targetLevel;

		static void CheckForProjectFile();
		static void CreateScene();
		static void LoadScene(const string& levelName);
		static void LoadGameObject(const map<string, string> obj);
		static void SaveScene(SaveType saveType = SaveType::defaultSave, const string& targetLevel = "");
		/// <summary>
		/// Export all scenes to current project folder when shutting down.
		/// </summary>
		static void ExportAllScenes();
	};
}