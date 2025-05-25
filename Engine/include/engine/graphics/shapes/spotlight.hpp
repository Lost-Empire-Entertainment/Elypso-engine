//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <string>
#include <filesystem>

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
	using std::filesystem::path;

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
			const string& txtFilePath = "",
			const bool& canCastShadows = true,
			const vec3& diffuse = vec3(1),
			const float& intensity = 1.0f,
			const float& farPlane = 1.0f,
			const float& nearPlane = 0.5f,
			const float& innerAngle = 12.5f,
			const float& outerAngle = 17.5f,
			string& name = tempName,
			unsigned int& id = tempID,
			const bool& isEnabled = true,

			//
			// BILLBOARD VALUES
			//

			unsigned int& billboardID = tempID,
			const bool& isBillboardEnabled = true);

		static void RenderSpotLight(
			const shared_ptr<GameObject>& obj, 
			const mat4& view, 
			const mat4& projection);
	};
}