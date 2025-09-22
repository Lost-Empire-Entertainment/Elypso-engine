//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace EngineFile
{
	using std::string;

	class GameObjectFile
	{
	public:
		/// <summary>
		/// Save all current scene gameobjects.
		/// </summary>
		static void SaveGameObjects();

		/// <summary>
		/// Handle the loading of the gameobjects folder.
		/// </summary>
		static void LoadGameObjects();

		static void LoadModel(const string& file);
	private:
		static string GetType(const string& file);

		static void LoadEmpty(const string& file);
		static void LoadAudioObject(const string& file);
		static void LoadCameraObject(const string& file);
		static void LoadPointLight(const string& file);
		static void LoadSpotlight(const string& file);
		static void LoadDirectionalLight(const string& file);
	};
}