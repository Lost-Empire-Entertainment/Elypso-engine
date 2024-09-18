//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//external
#include "glm.hpp"

//game
#include "gameobject.hpp"

using Graphics::Shape::GameObject;

using glm::vec3;
using glm::mat4;

namespace Graphics::Shape
{
	class Billboard
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> InitializeBillboard(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& vertShader = "",
			const string& fragShader = "",
			const string& diffTexture = "",
			const float& shininess = 32,
			string& name = tempName,
			unsigned int& id = tempID,
			const bool& isEnabled = true);

		static void RenderBillboard(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection);
	};
}