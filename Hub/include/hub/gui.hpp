//<Elypso hub>
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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine 

#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class GUI
{
public:
	enum class SelectType
	{
		new_folder,
		existing_file,
		engine_path,
		scene_file
	};

	static inline bool foundInvalidPath;

	static inline bool renderConfirmWindow;
	static inline string confirmFileName;
	static inline string confirmFilePath;

	static inline int framebufferWidth, framebufferHeight;
	static inline float panelSpacing = 10.0f;
	static inline float panelHeight = 200.0f;

	static inline vector<string> files;

	static void Initialize();

	static void Render();
	static void RenderPanels();
	static void RenderButtons();

	static void NewProject();
	static void AddProject();

	static void SetEnginePathBySelection();
	static void SetEnginePathFromConfigFile();

	static void RemoveProject(const string& projectName);
	static void ConfirmRemove(const string& projectName, const string& projectPath);

	static bool IsValidEnginePath(const string& enginePath);

	static void RunProject(const string& targetProject);

	static void RunApplication(const string& parentFolderPath, const string& exePath, const string& commands = "");

	static string SelectWithExplorer(enum SelectType);

	static void UpdateFileList();

	static void Shutdown();
};