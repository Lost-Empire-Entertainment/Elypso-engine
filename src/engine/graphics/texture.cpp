//external
#include "glad.h"
#include "glfw3.h"
#include "stb_image.h"

//engine
#include "texture.h"
#include "console.h"

#include <iostream>

namespace Graphics
{
	std::vector<unsigned int> Texture::textures;

	Texture::Texture(const std::string& path) : texturePath(path){}

	void Texture::LoadTexture()
	{
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::TEXTURE,
			Core::Console::ConsoleManager::Type::INFO,
			"Initializing texture...\n");

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
		unsigned char* data = stbi_load(Texture::texturePath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			textures.push_back(texture);

			Core::Console::ConsoleManager::WriteConsoleMessage(
				Core::Console::ConsoleManager::Caller::TEXTURE,
				Core::Console::ConsoleManager::Type::SUCCESS,
				"Texture initialized successfully!\n\n");
		}
		else
		{
			Core::Console::ConsoleManager::WriteConsoleMessage(
				Core::Console::ConsoleManager::Caller::TEXTURE,
				Core::Console::ConsoleManager::Type::ERROR,
				"Failed to load texture!");
		}
		stbi_image_free(data);
	}
}