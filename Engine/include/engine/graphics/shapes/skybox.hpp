//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>
#include <memory>

//external
#include "../../../../_external_shared/GLM/glm.hpp"

//engine
#include "gameobject.hpp"
#include "core.hpp"

namespace Graphics::Shape
{
	using glm::vec3;
	using glm::mat4;
	using std::vector;
	using std::string;
	using std::shared_ptr;

	using Graphics::Shape::GameObject;
	using Core::Engine;

	class Skybox
	{
	public:
		static inline string name = "Skybox";
		static inline unsigned int id = 10000002;

		static shared_ptr<GameObject> InitializeSkybox(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& vertShader = Engine::filesPath + "\\shaders\\Basic_model.vert",
			const string& fragShader = Engine::filesPath + "\\shaders\\Basic.frag",
			string& thisName = name,
			unsigned int& thisId = id,
			const bool& isEnabled = true);

		static void RenderSkybox(
			const shared_ptr<GameObject>& obj,
			mat4& view,
			const mat4& projection);

		/// <summary>
		/// Loads a cubemap texture from 6 individual texture faces.
		/// Order:
		/// +X (right),
		/// -X (left),
		/// +Y (top),
		/// -Y (bottom),
		/// +Z (front),
		/// -Z (back)
		/// </summary>
		static void AssignSkyboxTextures(vector<string> textures, bool flipTextures);
	private:
		static inline unsigned int skyboxTexture;
	};
}