//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <map>
#include <memory>
#include <string>

//game
#include "gameobject.hpp"

using std::map;
using std::shared_ptr;
using std::string;

using Graphics::Shape::GameObject;

namespace GameFile
{
	class GameObjectFile
	{
	public:
		static void SaveGameObject(const shared_ptr<GameObject>& obj);
		static void LoadGameObject(const map<string, string>& data, const string& folderPath);
		static void LoadGameObjects(const string& gameObjectsPath);
	};
}