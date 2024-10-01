//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <string>

//external
#include "glm.hpp"

//engine
#include "core.hpp"

namespace Graphics::Shape
{
	class GameObject;

	using glm::vec3;
	using glm::mat4;
	using std::shared_ptr;
	using std::string;

	using Core::Engine;

	class SpotLight
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> InitializeSpotLight(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& modelPath = "",
			const string& vertShader = Engine::filesPath + "\\shaders\\Basic_model.vert",
			const string& fragShader = Engine::filesPath + "\\shaders\\Basic.frag",
			const vec3& diffuse = vec3(1),
			const float& intensity = 1.0f,
			const float& distance = 1.0f,
			const float& innerAngle = 12.5f,
			const float& outerAngle = 17.5f,
			string& name = tempName,
			unsigned int& id = tempID,
			const bool& isEnabled = true,

			//
			// BILLBOARD VALUES
			//

			const string& billboardVertShader = Engine::filesPath + "\\shaders\\Basic_texture.vert",
			const string& billboardFragShader = Engine::filesPath + "\\shaders\\Basic_texture.frag",
			const string& billboardDiffTexture = Engine::filesPath + "\\icons\\spotLight.png",
			const float& billboardShininess = 32,
			string& billboardName = tempName,
			unsigned int& billboardID = tempID);

		static void RenderSpotLight(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection);
	};
}