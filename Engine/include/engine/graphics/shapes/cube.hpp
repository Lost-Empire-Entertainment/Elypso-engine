//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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

using Graphics::Shape::GameObject;

namespace Graphics::Shape
{
	class Cube
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> InitializeCube(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& vertShader = "shaders/GameObject.vert",
			const string& fragShader = "shaders/GameObject.frag",
			const string& diffTexture = "textures/crate_2.png",
			const string& specTexture = "textures/crate_2_specular.png",
			const float& shininess = 32,
			string& name = tempName,
			unsigned int& id = tempID);

		static void RenderCube(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection);
	};
}