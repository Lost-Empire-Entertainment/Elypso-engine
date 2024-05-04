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
#include "gameobject.hpp"
#include "core.hpp"

using std::shared_ptr;
using std::string;
using glm::vec3;
using glm::mat4;

using Graphics::Shape::GameObject;
using Core::Engine;

namespace Graphics::Shape
{
	class ModelChild
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> Initialize(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& vertShader = Engine::filesPath + "/shaders/GameObject.vert",
			const string& fragShader = Engine::filesPath + "/shaders/GameObject.frag",
			const string& diffTexture = Engine::filesPath + "/textures/default_diffuse.png",
			const string& specTexture = "EMPTY",
			const string& normalTexture = "EMPTY",
			const string& heightTexture = "EMPTY",
			const vector<AssimpVertex> vertices = {},
			const vector<unsigned int> indices = {},
			const float& shininess = 32,
			const map<GameObject::Category, bool> categories = {},
			string& name = tempName,
			unsigned int& id = tempID);

		static void Render(
			const shared_ptr<GameObject>& obj,
			const mat4& view,
			const mat4& projection);
	};
}