//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

using std::string;

namespace Core
{
	class Engine
	{
	public:
		static inline string name = "Elypso engine";
		static inline string version = "0.0.14 Prototype";
		static inline string docsPath;
		static inline string filesPath;
		static inline string projectPath;
		static inline string gamePath;
		static inline string gameExePath;
		static inline string gameParentPath;

		static inline bool startedWindowLoop;

		static void InitializeEngine();
		static void RunEngine();

		static void CreateErrorPopup(const char* errorTitle = "INSERT ERROR TITLE", const char* errorMessage = "INSERT ERROR MESSAGE");

		static bool IsThisProcessAlreadyRunning(const string& processName);

		/// <param name="immediate">Does the engine need to be shut down immediately?</param>
		static void Shutdown(bool immediate = false);
	};
}