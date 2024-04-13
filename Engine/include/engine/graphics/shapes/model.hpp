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
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "core.hpp"

//engine
#include "gameobject.hpp"

using std::shared_ptr;
using std::string;
using glm::vec3;
using glm::mat4;

using Graphics::Shape::GameObject;
using Core::Engine;

namespace Graphics::Shape
{
	class Model
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;
		static inline string targetModel;

		static void Initialize(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& modelPath = targetModel,
			const string& vertShader = Engine::filesPath + "/shaders/GameObject.vert",
			const string& fragShader = Engine::filesPath + "/shaders/GameObject.frag",
			const string& diffTexture = Engine::filesPath + "/textures/default_diffuse.png",
			const string& specTexture = "EMPTY",
			const string& normalTexture = "EMPTY",
			const string& heightTexture = "EMPTY",
			const float& shininess = 32,
			string& name = tempName,
			unsigned int& id = tempID);

		static void ProcessNode(
			string& name,
			unsigned int& id,
			bool isModelChild,
			const vec3& pos,
			const vec3& rot,
			const vec3& scale,
			const string& modelPath,
			const string& vertShader,
			const string& fragShader,
			const string& diffTexture,
			const string& specTexture,
			const string& normalTexture,
			const string& heightTexture,
			const float& shininess,
			aiNode* node,
			const aiScene* scene);

		static AssimpMesh ProcessMesh(
			aiMesh* mesh,
			const aiScene* scene);
	};
}