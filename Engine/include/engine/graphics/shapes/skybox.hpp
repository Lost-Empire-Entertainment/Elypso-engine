//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>

//external
#include "glm.hpp"

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
	using std::filesystem::path;

	using Graphics::Shape::GameObject;
	using Core::Engine;

	class Skybox
	{
	public:
		static shared_ptr<GameObject> InitializeSkybox();

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