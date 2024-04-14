//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//engine
#include "selectobject.hpp"
#include "gameobject.hpp"

using Graphics::Shape::GameObject;
using Physics::Select;

namespace Physics
{
	class Collision
	{
	public:
		static bool IsRayIntersectingCube(const Select::Ray& ray, const shared_ptr<GameObject>& cube);
	};
}