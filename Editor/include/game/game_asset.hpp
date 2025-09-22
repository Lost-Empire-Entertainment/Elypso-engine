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
		/// <summary>
		/// Returns gameobject with given name if it exists.
		/// </summary>
		static shared_ptr<GameObject> GetGameObject(const string& name);
		/// <summary>
		/// Returns camera of the current scene.
		/// </summary>
		static shared_ptr<GameObject> GetCamera();
	};
}