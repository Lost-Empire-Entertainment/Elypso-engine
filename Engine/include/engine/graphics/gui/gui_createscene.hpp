//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <string>

namespace Graphics::GUI
{
	using std::string;

	class GUICreateScene
	{
	public:
		static inline bool renderCreateSceneWindow;

		static void RenderCreateSceneWindow();
	private:
		static inline string assignedSceneName = "Scene";
		static constexpr int bufferSize = 16;

		static inline char sceneName[bufferSize];

		static void RenderCreateSceneWindowContent();
	};
}
#endif