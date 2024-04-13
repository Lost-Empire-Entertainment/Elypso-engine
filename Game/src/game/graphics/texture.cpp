//Copyright(C) < 2024 > < Lost Empire Entertainment >

#include <string>
#include <vector>
#include <iostream>

//external
#include "glfw3.h"
#include "stb_image.h"

//game
#include "texture.hpp"

using std::cout;
using std::endl;

namespace Graphics
{
	void Texture::LoadTexture(
		const shared_ptr<GameObject>& obj,
		const string& texturePath,
		const Material::TextureType type,
		bool flipTexture)
	{
		//the texture already exists and has already been assigned to this model once
		if (obj->GetMaterial()->TextureExists(texturePath)
			&& texturePath != "EMPTY")
		{
			return;
		}

		//the texture is EMPTY and is just a placeholder
		if (texturePath == "EMPTY")
		{
			obj->GetMaterial()->AddTexture(texturePath, 0, type);
			return;
		}

		//the texture exists but hasnt yet been added to this model
		auto it = textures.find(texturePath);
		if (it != textures.end())
		{
			obj->GetMaterial()->AddTexture(texturePath, it->second, type);
			return;
		}

		//cout << "Initializing texture " << texturePath << "...\n";

		//the texture does not yet exist

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
		int width, height, nrComponents{};
		stbi_set_flip_vertically_on_load(flipTexture);
		unsigned char* data = stbi_load((texturePath).c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format{};
			if (nrComponents == 1) format = GL_RED;
			else if (nrComponents == 3) format = GL_RGB;
			else if (nrComponents == 4) format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			obj->GetMaterial()->AddTexture(texturePath, texture, type);

			textures[texturePath] = texture;

			//cout << "Added new texture " << texturePath << " to " << obj->GetName() << "! New texture count is " << obj->GetMaterial()->GetTextureCount() << ".\n\n";
		}
		else
		{
			cout << "Error: Failed to load " << texturePath << " texture!\n\n";
		}
		stbi_image_free(data);
	}
}