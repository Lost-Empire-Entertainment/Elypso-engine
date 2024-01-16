//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#include "gameobject.hpp"

using Type = Graphics::Shape::MeshType;


namespace Graphics::Shape
{
	GameObject* GameObjectManager::CreateObject(const string& name, const unsigned int& ID)
	{
		objects.push_back(make_unique<GameObject>(GameObject{
			name,
			ID}));
		return objects.back().get();
	}

	void GameObjectManager::DestroyObject(GameObject* obj)
	{

	}
}