//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

using std::string;

namespace Core
{
	class Game
	{
	public:
		static inline string gameName;
		static inline string gameVersion;

		//internal paths
		static inline string docsPath;
		static inline string projectPath;
		static inline string filesPath;
		static inline string texturesPath;

		//external paths
		static inline string externalFilesPath;
		static inline string scenePath;
		static inline string scenesPath;

		static void Initialize();
		static void Run();
		static void Shutdown(bool immediate = false);

		static bool IsThisProcessAlreadyRunning(const string& processName);
		static void CreateErrorPopup(
			const char* errorTitle = "INSERT ERROR TITLE", 
			const char* errorMessage = "INSERT ERROR MESSAGE");
	};
}