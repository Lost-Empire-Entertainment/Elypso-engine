//This script is a part of the Game Template that is distributed by Elypso Engine.
//If you have access to this script then you have full freedom to do anything with it.
//A copy of the MIT license has been included in LICENSE.md

#pragma once

#include <string>

using std::string;

namespace GameFile
{
	class SceneFile
	{
	public:
		static void CheckForProjectFile();

		static void SaveScene();
		static void LoadScene(const string& scene);
	};
}