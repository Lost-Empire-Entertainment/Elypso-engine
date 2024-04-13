//Copyright(C) < 2024 > < Lost Empire Entertainment >

#pragma once

#include <string>

using std::string;

namespace Core
{
	class Game
	{
	public:
		static inline string name = "Game";
		static inline string version = "0.0.1";
		static inline string docsPath;
		static inline string filesPath;

		static inline bool startedWindowLoop;

		static void InitializeGame();
		static void RunGame();

		static void CreateErrorPopup(const char* errorTitle = "INSERT ERROR TITLE", const char* errorMessage = "INSERT ERROR MESSAGE");

		static void Shutdown();
	};
}