//This script is a part of the Game Template that is distributed by Elypso Engine.
//If you have access to this script then you have full freedom to do anything with it.
//A copy of the MIT license has been included in LICENSE.md

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