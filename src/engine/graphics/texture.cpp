//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//external
#include "glad.h"
#include "glfw3.h"
#include "stb_image.h"

//engine
#include "console.hpp"
#include "texture.hpp"

using namespace std;
using namespace Core;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Graphics
{
	vector<unsigned int> Texture::textures;

	Texture::Texture(const string& path) : texturePath(path){}

	void Texture::LoadTexture(const string& textureName, bool flipTexture, GLenum pixelFormat)
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::TEXTURE,
			Type::INFO,
			"Initializing texture " + textureName + "...\n");

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//set texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//load image, create texture and generate mipmaps
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(flipTexture);
		unsigned char* data = stbi_load((Texture::texturePath + "\\" + textureName).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			textures.push_back(texture);

			ConsoleManager::WriteConsoleMessage(
				Caller::TEXTURE,
				Type::SUCCESS,
				"Texture " + textureName + " initialized successfully!\n\n");
		}
		else
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::TEXTURE,
				Type::EXCEPTION,
				"Failed to load " + textureName + " texture!\n\n");
		}
		stbi_image_free(data);
	}
}