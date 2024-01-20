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

#include "glm.hpp"
#include "gameobject.hpp"

using glm::vec3;

using Graphics::Shape::GameObject;

namespace Graphics::Shape
{
	class Cube : public GameObject
	{
	public:
		static shared_ptr<GameObject> InitializeCube(const vec3& pos, const vec3& rot, const vec3& scale);

		virtual void Render(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection) const override;
	};
}