//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>

//external
#include "glm.hpp"

namespace Graphics::Shape
{
	class GameObject;

	using glm::vec3;
	using glm::mat4;
	using std::string;
	using std::shared_ptr;

	class AudioObject
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> InitializeAudioObject(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& txtFilePath = "",
			string& name = tempName,
			unsigned int& id = tempID,
			const bool& isEnabled = true,

			//
			// AUDIO COMPONENT VALUES
			//

			const string& audioFileName = "",
			const bool& isPlaying = false,
			const bool& is3D = false,
			const float& volume = 50.0f,
			const float& minRange = 1.0f,
			const float& maxRange = 100.0f,

			//
			// BILLBOARD VALUES
			//

			unsigned int& billboardID = tempID,
			const bool& isBillboardEnabled = true);

		static void RenderAudioObject(
			const shared_ptr<GameObject>& obj,
			const mat4& view,
			const mat4& projection);
	};
}