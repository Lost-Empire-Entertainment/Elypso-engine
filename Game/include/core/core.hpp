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

		static inline string docsPath;
		static inline string filesPath;
		static inline string scenePath;
		static inline string scenesPath;
		static inline string texturesPath;
		static inline string projectPath;

		static void Initialize();
		static void Run();
		static void Shutdown();

		static bool IsThisProcessAlreadyRunning(const string& processName);
		static void CreateErrorPopup(const string& title, const string& message);
	};
}