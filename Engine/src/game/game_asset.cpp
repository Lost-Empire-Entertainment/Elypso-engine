//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <vector>
#include <memory>

//engine
#include "gameobject.hpp"

//game
#include "game_asset.hpp"

using std::vector;

using Graphics::Shape::GameObjectManager;

namespace Game
{
	shared_ptr<GameObject> Game_Asset::GetGameObject(const string& name)
	{
		vector<shared_ptr<GameObject>> gameobjects = GameObjectManager::GetObjects();

		for (const auto& obj : gameobjects)
		{
			string objName = obj->GetName();
			if (objName == name)
			{
				return obj;
			}
		}
		return nullptr;
	}
}