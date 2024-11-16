//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <string>

//external
#include "../../../../_external_shared/GLM/glm.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "core.hpp"

//engine
#include "gameobject.hpp"

namespace Graphics::Shape
{
	using std::shared_ptr;
	using std::string;
	using glm::vec3;
	using glm::mat4;

	using Graphics::Shape::GameObject;
	using Core::Engine;

	class Importer
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static void Initialize(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& modelPath = "",
			const string& vertShader = Engine::filesPath + "\\shaders\\GameObject.vert",
			const string& fragShader = Engine::filesPath + "\\shaders\\GameObject.frag",
			const string& diffTexture = "DEFAULTDIFF",
			const string& specTexture = "DEFAULTSPEC",
			const string& normalTexture = "EMPTY",
			const string& heightTexture = "EMPTY",
			const float& shininess = 32,
			string& name = tempName,
			unsigned int& id = tempID,
			const bool& isEnabled = true);

		static void ProcessNode(
			string& name,
			unsigned int& id,
			const bool& isEnabled,
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

		static void DecomposeTransform(const aiMatrix4x4& transform, vec3& outPosition, vec3& outRotation, vec3& outScale);
	private:
		static bool ValidateScene(const aiScene* scene);

		//check mesh data

		static int AnimMeshCount(const aiMesh* scene);
		static int BoneCount(const aiMesh* scene);
		static int VerticeCount(const aiMesh* scene);

		//check scene data

		static int AnimationCount(const aiScene* scene);
		static int CameraCount(const aiScene* scene);
		static int LightCount(const aiScene* scene);
		static int MaterialCount(const aiScene* scene);
		static int MeshCount(const aiScene* scene);
		static int SkeletonCount(const aiScene* scene);
		static int TextureCount(const aiScene* scene);
	};
}