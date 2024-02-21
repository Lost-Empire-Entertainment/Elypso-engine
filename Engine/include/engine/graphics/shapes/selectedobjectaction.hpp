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
#include "gameobject.hpp"

using glm::vec3;
using glm::mat4;

using Graphics::Shape::GameObject;

namespace Graphics::Shape
{
	class ActionTex
	{
	public:
		static shared_ptr<GameObject> InitializeActionTex(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1));

		static void RenderActionTex(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection);
	private:
		static inline vec3 x = vec3(90.0f, 0.0f, 0.0f);
		static inline vec3 y = vec3(0.0f, 90.0f, 0.0f);
		static inline vec3 z = vec3(0.0f, 0.0f, 90.0f);
	};
}