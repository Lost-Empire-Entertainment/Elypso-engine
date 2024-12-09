//Copyright(C) 2024 Lost Empire Entertainment
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
#include "gameobject.hpp"
#include "core.hpp"

namespace Graphics::Shape
{
	using std::shared_ptr;
	using std::string;
	using glm::vec3;
	using glm::mat4;
	using std::filesystem::path;

	using Graphics::Shape::GameObject;
	using Core::Engine;

	class Model
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> Initialize(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& txtFilePath = "",
			const string& modelPath = "",
			const string& diffTexture = "DEFAULTDIFF",
			const string& specTexture = "DEFAULTSPEC",
			const string& normalTexture = "EMPTY",
			const string& heightTexture = "EMPTY",
			const vector<AssimpVertex> vertices = {},
			const vector<unsigned int> indices = {},
			string& name = tempName,
			unsigned int& id = tempID,
			const bool& isEnabled = true,
			const bool& isMeshEnabled = true);

		static void Render(
			const shared_ptr<GameObject>& obj,
			const mat4& view,
			const mat4& projection);
	};
}
