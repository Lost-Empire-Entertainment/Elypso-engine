#pragma once

#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

//external
#include "glad.h"

#include <string>
#include <vector>

using namespace std;

namespace Core::Graphics
{
	class Texture
	{
	public:
		static vector<unsigned int> textures;
		const string texturePath;
		Texture(const string& path);

		/// <summary>
		/// Handles the loading of each game texture.
		/// </summary>
		/// <param name="textureName">What is the actual texture name we are trying to load?</param>
		/// <param name="flipTexture">Should the texture be flipped 180 degrees?</param>
		/// <param name="pixelFormat">What is the pixel format of the texture? (GL_RGB/GL_RGBA etc.)</param>
		void LoadTexture(const string& textureName, bool flipTexture = false, GLenum pixelFormat = GL_RGB);
	};
}

#endif