//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

using std::string;

namespace Core
{
	class Compiler
	{
	public:
		static inline string docsPath;
		static inline string filesPath;
		static inline string configFilePath;
		static inline string projectsPath;

		static void MainInitialize();

		static bool IsThisProcessAlreadyRunning(const string& processName);
		static void CreateErrorPopup(const string& message);

		static void MainLoop();
		static void MainShutdown();
	private:
		static inline bool isCompilerRunning;
	};
}