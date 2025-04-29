//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <string>

#include "gameobject.hpp"

namespace Game
{
	using std::string;
	using std::shared_ptr;

	using Graphics::Components::GameObject;

	class Game_Asset
	{
	public:
		static shared_ptr<GameObject> GetGameObject(const string& name);
	};
}