//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>

//external
#include "glm.hpp"

//engine
#include "core.hpp"

namespace Graphics::Shape
{
	class GameObject;

	using glm::vec3;
	using glm::mat4;
	using std::string;
	using std::shared_ptr;

	using Core::Engine;

	class DirectionalLight
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> InitializeDirectionalLight(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& modelPath = "",
			const string& vertShader = Engine::filesPath + "\\shaders\\Basic_model.vert",
			const string& fragShader = Engine::filesPath + "\\shaders\\Basic.frag",
			const vec3& diffuse = vec3(1),
			const float& intensity = 1.0f,
			string& name = tempName,
			unsigned int& id = tempID,
			const bool& isEnabled = true,
			const bool& isMeshEnabled = true,

			//
			// BILLBOARD VALUES
			//

			const string& billboardVertShader = Engine::filesPath + "\\shaders\\Basic_texture.vert",
			const string& billboardFragShader = Engine::filesPath + "\\shaders\\Basic_texture.frag",
			const string& billboardDiffTexture = Engine::filesPath + "\\icons\\directionalLight.png",
			const float& billboardShininess = 32,
			string& billboardName = tempName,
			unsigned int& billboardID = tempID,
			const bool& isBillboardEnabled = true);

		static void RenderDirectionalLight(
			const shared_ptr<GameObject>& obj, 
			const mat4& view, 
			const mat4& projection);
	};
}