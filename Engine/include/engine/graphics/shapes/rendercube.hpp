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

#pragma once

//external
#include "glm.hpp"

//engine
#include "rendercube.hpp"
#include "gameobject.hpp"

#include <vector>

using std::vector;

using Core::ECS::GameObject;

using glm::vec3;
using glm::mat4;

namespace Graphics::Shapes
{
	class Cube
	{
	public:
		static GameObject CreateCube(const vec3& position, const vec3& scale, const vec3& color, float shininess);
		static void RenderCube(GameObject& obj, mat4& view, mat4& projection);
	};
}