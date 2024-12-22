//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <unordered_map>
#include <memory>

//external
#include "glad.h"

//engine
#include "gameobject.hpp"
#include "materialcomponent.hpp"

namespace Graphics
{
	using std::unordered_map;
	using std::shared_ptr;

	using Graphics::Shape::GameObject;
	using Graphics::Components::MaterialComponent;

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
			const MaterialComponent::TextureType type = MaterialComponent::TextureType::diffuse,
			bool flipTexture = false);
		static void DeleteTexture(unsigned int texture)
		{
			glDeleteTextures(1, &texture);
		}

	private:
		static inline unordered_map<string, unsigned int> textures;
	};
}