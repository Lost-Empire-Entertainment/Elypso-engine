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
#include "gameobject.hpp"

namespace Graphics::Shape
{
	using std::string;
	using std::shared_ptr;
	using glm::vec3;

	using Graphics::Shape::GameObject;

	class Empty
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> InitializeEmpty(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& txtFilePath = "",
			string& name = tempName,
			unsigned int& id = tempID,
			const bool& isEnabled = true);
	};
}