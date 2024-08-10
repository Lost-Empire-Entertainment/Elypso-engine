//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

//external
#include "glfw3.h"
#include "stb_image.h"

//game
#include "texture.hpp"
#include "console.hpp"
#include "core.hpp"
#include "fileUtils.hpp"

using std::cout;
using std::endl;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::filesystem::exists;

using Core::Console;
using Caller = Core::Console::Caller;
using Type = Core::Console::Type;
using Core::Game;
using Utils::File;
using Graphics::Shape::Mesh;

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
			&& texturePath != "DEFAULTDIFF"
			&& texturePath != "DEFAULTSPEC"
			&& texturePath != "EMPTY")
		{
			return;
		}

		//the texture is a default diffuse texture and uses the placeholder diffuse texture
		if (texturePath == "DEFAULTDIFF")
		{
			string defaultTexturePath = Game::filesPath + "\\textures\\diff_default.png";
			auto it = textures.find(defaultTexturePath);
			if (it != textures.end())
			{
				obj->GetMaterial()->AddTexture(defaultTexturePath, it->second, type);
				return;
			}
		}

		//the texture is a default specular texture and uses the placeholder specular texture
		if (texturePath == "DEFAULTSPEC")
		{
			string defaultTexturePath = Game::filesPath + "\\textures\\spec_default.png";
			auto it = textures.find(defaultTexturePath);
			if (it != textures.end())
			{
				obj->GetMaterial()->AddTexture(defaultTexturePath, it->second, type);
				return;
			}
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

		string finalTexturePath;
		if (texturePath == "DEFAULTDIFF")
		{
			finalTexturePath = Game::filesPath + "\\textures\\diff_default.png";
		}
		else if (texturePath == "DEFAULTSPEC")
		{
			finalTexturePath = Game::filesPath + "\\textures\\spec_default.png";
		}
		else if (obj->GetMesh()->GetMeshType() != Mesh::MeshType::model
			|| texturePath.find("diff_default.png") != string::npos
			|| texturePath.find("spec_default.png") != string::npos
			|| texturePath.find("diff_missing.png") != string::npos)
		{
			finalTexturePath = texturePath;
		}
		else
		{
			string copiedTexturePath;
			for (const auto& folder : directory_iterator(Game::currentGameobjectsPath))
			{
				string objName = obj->GetName();
				string folderName = path(folder).stem().string();

				if (folderName == objName)
				{
					string textureFolder = path(folder).string();
					string textureName = "\\" + path(texturePath).filename().string();
					copiedTexturePath = textureFolder + textureName;

					if (!exists(copiedTexturePath))
					{
						File::CopyFileOrFolder(texturePath, copiedTexturePath);
					}

					break;
				}
			}

			finalTexturePath = copiedTexturePath;
		}

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
		unsigned char* data = stbi_load((finalTexturePath).c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format{};
			if (nrComponents == 1) format = GL_RED;
			else if (nrComponents == 3) format = GL_RGB;
			else if (nrComponents == 4) format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			obj->GetMaterial()->AddTexture(finalTexturePath, texture, type);

			textures[finalTexturePath] = texture;
		}
		else
		{
			Console::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Failed to load texture '" + finalTexturePath + "'!\n\n");
		}
		stbi_image_free(data);
	}
}