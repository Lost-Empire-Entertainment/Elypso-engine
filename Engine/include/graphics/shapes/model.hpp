//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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
			const string& vertShader = Engine::filesPath + "\\shaders\\GameObject.vert",
			const string& fragShader = Engine::filesPath + "\\shaders\\GameObject.frag",
			const string& diffTexture = "DEFAULTDIFF",
			const string& specTexture = "DEFAULTSPEC",
			const string& normalTexture = "EMPTY",
			const string& heightTexture = "EMPTY",
			const float& shininess = 32,
			string& name = tempName,
			unsigned int& id = tempID);

		static void ProcessNode(
			string& name,
			unsigned int& id,
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