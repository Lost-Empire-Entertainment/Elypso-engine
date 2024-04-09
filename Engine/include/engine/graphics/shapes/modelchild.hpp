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

#include <memory>
#include <string>

//external
#include "glm.hpp"

//engine
#include "gameobject.hpp"
#include "core.hpp"

using std::shared_ptr;
using std::string;
using glm::vec3;
using glm::mat4;

using Graphics::Shape::GameObject;
using Core::Engine;

namespace Graphics::Shape
{
	class ModelChild
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> Initialize(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& vertShader = Engine::filesPath + "/shaders/GameObject.vert",
			const string& fragShader = Engine::filesPath + "/shaders/GameObject.frag",
			const string& diffTexture = "EMPTY",
			const string& specTexture = "EMPTY",
			const string& normalTexture = "EMPTY",
			const string& heightTexture = "EMPTY",
			const vector<AssimpVertex> vertices = {},
			const vector<unsigned int> indices = {},
			const float& shininess = 32,
			string& name = tempName,
			unsigned int& id = tempID);

		static void Render(
			const shared_ptr<GameObject>& obj,
			const mat4& view,
			const mat4& projection);
	};
}