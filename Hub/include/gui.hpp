//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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
		folder,
		engine_path,
		txt_file
	};

	static inline bool foundInvalidPath;

	static inline int framebufferWidth, framebufferHeight;
	static inline float panelSpacing = 10.0f;
	static inline float panelHeight = 200.0f;

	static inline vector<string> files;

	static void Initialize();

	static void Render();
	static void RenderPanels();
	static void RenderButtons();

	static void NewProject();
	static void SetProjectsFolder();
	static void SetEnginePath();

	static void RemoveProject(const string& projectName);

	static bool IsValidEnginePath(const string& enginePath);

	static void RunProject(const string& targetProject);

	static void RunApplication(const string& parentFolderPath, const string& exePath, const string& commands = "");

	static string SelectWithExplorer(enum SelectType);

	static void UpdateFileList();

	static void Shutdown();
};