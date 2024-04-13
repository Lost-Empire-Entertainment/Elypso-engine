//Copyright(C) < 2024 > < Lost Empire Entertainment >

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
			unsigned int& id = tempID);

		static void RenderBillboard(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection);
	};
}