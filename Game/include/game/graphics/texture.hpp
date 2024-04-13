//Copyright(C) < 2024 > < Lost Empire Entertainment >

#pragma once

#include <unordered_map>

//external
#include "glad.h"

//game
#include "gameobject.hpp"

using std::unordered_map;

using Graphics::Shape::GameObject;
using Graphics::Shape::Material;

namespace Graphics
{
	class Texture
	{
	public:
		/// <summary>
		/// Handles the loading of each game texture.
		/// </summary>
		/// <param name="obj">The target gameobject</param>
		/// <param name="texturePath">Full path to the texture file</param>
		/// <param name="type">What kind of a texture is it</param>
		/// <param name="flipTexture">Should the texture be flipped 180 degrees</param>
		static void LoadTexture(
			const shared_ptr<GameObject>& obj,
			const string& texturePath,
			const Material::TextureType type = Material::TextureType::diffuse,
			bool flipTexture = false);
		static void DeleteTexture(unsigned int texture)
		{
			glDeleteTextures(1, &texture);
		}

	private:
		static inline unordered_map<string, unsigned int> textures;
	};
}