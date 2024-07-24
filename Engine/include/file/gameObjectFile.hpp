//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <string>
#include <map>

#include "gameobject.hpp"

using std::string;
using std::shared_ptr;
using std::map;

using Graphics::Shape::GameObject;
using Graphics::Shape::Mesh;

namespace EngineFile
{
	class GameObjectFile
	{
	public:
		/// <summary>
		/// Save gameobject data to project folder.
		/// </summary>
		/// <param name="obj"></param>
		static void SaveGameObject(const shared_ptr<GameObject>& obj);

		/// <summary>
		/// Handle the loading of the gameobjects folder.
		/// </summary>
		/// <param name="path"></param>
		static void LoadGameObjects(const string& path);
	private:
		/// <summary>
		/// Loads each individual gameobject.
		/// </summary>
		/// <param name="data">Data map for this gameobject</param>
		/// <param name="folderPath">The name of the folder of this gameobject</param>
		static void LoadGameObject(const map<string, string>& data, const string& folderPath);
	};
}