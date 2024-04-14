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
		static inline string version = "0.0.13 Prototype";
		static inline string docsPath;
		static inline string filesPath;
		static inline string gamePath;

		static inline bool startedWindowLoop;

		static void InitializeEngine();
		static void RunEngine();

		static void CreateErrorPopup(const char* errorTitle = "INSERT ERROR TITLE", const char* errorMessage = "INSERT ERROR MESSAGE");

		static void Shutdown();
	};
}