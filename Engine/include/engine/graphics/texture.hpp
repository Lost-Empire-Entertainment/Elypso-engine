//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#pragma once

#include <unordered_map>

//external
#include "glad.h"

//engine
#include "gameobject.hpp"

using std::unordered_map;

using Graphics::Shape::GameObject;

namespace Graphics
{
	class Texture
	{
	public:
		const string texturePath;
		Texture(const string& path);

		/// <summary>
		/// Handles the loading of each game texture.
		/// </summary>
		/// <param name="textureName">What is the actual texture name we are trying to load?</param>
		/// <param name="flipTexture">Should the texture be flipped 180 degrees?</param>
		/// <param name="pixelFormat">What is the pixel format of the texture? (GL_RGB/GL_RGBA etc.)</param>
		void LoadTexture(const shared_ptr<GameObject>& obj, const string& textureName, bool flipTexture = false, GLenum pixelFormat = GL_RGB);
		void DeleteTexture(unsigned int texture)
		{
			glDeleteTextures(1, &texture);
		}
	private:
		unordered_map<string, unsigned int> textures;
	};
}