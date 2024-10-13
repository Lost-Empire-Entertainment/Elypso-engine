//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <string>
#include <map>

#include "gameobject.hpp"

namespace EngineFile
{
	using std::string;
	using std::shared_ptr;
	using std::map;

	using Graphics::Shape::GameObject;

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
	private:
		static string GetType(const string& file);

		static void LoadModel(const string& file);
		static void LoadPointLight(const string& file);
		static void LoadSpotlight(const string& file);
		static void LoadDirectionalLight(const string& file);
	};
}