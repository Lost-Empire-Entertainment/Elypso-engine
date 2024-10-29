//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>
#include <string>
#include <vector>

//external
#include "imgui.h"

namespace Core
{
	using std::string;
	using std::filesystem::path;

	using std::vector;

	class Hub
	{
	public:
		static inline bool isHubRunning;

		static inline path defaultPath;

		static inline path docsPath;
		static inline path projectsFolderPath;

		static inline path configFilePath;
		static inline path enginePath;

		static void Initialize();

		static void LoadConfigFile();
		static void SaveConfigFile();
		static vector<string> StringSplit(const string& input, char delimiter);

		static bool IsThisProcessAlreadyRunning(const string& processName);
		static void CreateErrorPopup(const char* errorMessage = "INSERT ERROR MESSAGE");

		static void UpdateActivityTime();
		static bool IsUserIdle();

		static void Render();
		static void Shutdown();
	private:
		static bool IsInputActive();
	};
}