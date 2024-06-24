//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

using std::string;

namespace Graphics::GUI
{
	class GUICreateScene
	{
	public:
		static inline bool renderCreateSceneWindow;

		static void RenderCreateSceneWindow();
	private:
		static inline string assignedSceneName = "Scene";
		static constexpr int bufferSize = 32;

		static inline char sceneName[bufferSize];

		static void RenderCreateSceneWindowContent();
	};
}