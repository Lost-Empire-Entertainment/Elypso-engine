//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

//external
#include "../../../../_external_shared/GLFW/glfw3.h"
#include "../../../../_external_shared/Stb_image/stb_image.h"

//engine
#include "texture.hpp"
#include "console.hpp"
#include "core.hpp"
#include "fileUtils.hpp"

using std::cout;
using std::endl;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
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
			string defaultTexturePath = Engine::filesPath + "\\textures\\diff_default.png";
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
			string defaultTexturePath = Engine::filesPath + "\\textures\\spec_default.png";
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

		//default diff texture was assigned
		if (texturePath == "DEFAULTDIFF")
		{
			finalTexturePath = Engine::filesPath + "\\textures\\diff_default.png";
		}
		//default spec texture was assigned
		else if (texturePath == "DEFAULTSPEC")
		{
			finalTexturePath = Engine::filesPath + "\\textures\\spec_default.png";
		}
		//the texture path is assigned but doesnt exist, assigning missing texture
		else if (obj->GetMesh()->GetMeshType() != Mesh::MeshType::model
				 || texturePath.find("diff_default.png") != string::npos
				 || texturePath.find("spec_default.png") != string::npos
				 || texturePath.find("diff_missing.png") != string::npos)
		{
			finalTexturePath = texturePath;
		}
		//otherwise look for texture inside model folder
		else
		{
			bool foundObject = false;
			string copiedTexturePath;
			//first iteration to loop through all folders in gameobjects folder
			for (const auto& folder : directory_iterator(Engine::currentGameobjectsPath))
			{
				string objName = obj->GetName();
				string folderName = path(folder).stem().string();

				if (is_directory(folder)
					&& folderName == objName)
				{
					for (const auto& file : directory_iterator(folder))
					{
						//set the image file path
						string extension = path(file).extension().string();
						string fileName = path(file).filename().string();
						if (is_regular_file(file)
							&& texturePath == fileName)
						{
							finalTexturePath = path(file).string();
							foundObject = true;
							break;
						}
					}
				}
				if (foundObject) break;
			}
		}

		//texture was not found and was not assigned, assign brand new texture
		if (finalTexturePath == "") finalTexturePath = texturePath;

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

			string textureName = path(texturePath).filename().string();

			if (textureName.find("diff_default.png") == string::npos
				&& textureName.find("diff_missing.png") == string::npos
				&& textureName.find("pointLight.png") == string::npos
				&& textureName.find("spotLight.png") == string::npos
				&& textureName.find("directionalLight.png") == string::npos
				&& textureName.find("blank.png") == string::npos
				&& textureName.find("move.png") == string::npos
				&& textureName.find("rotate.png") == string::npos
				&& textureName.find("scale.png") == string::npos
				&& textureName.find("DEFAULTDIFF") == string::npos
				&& textureName.find("DEFAULTSPEC") == string::npos)
			{
				string objFolder = path(obj->GetTxtFilePath()).parent_path().string();
				string originPath = Engine::texturesPath + "\\" + textureName;
				string targetPath = objFolder + "\\" + textureName;

				if (!exists(targetPath))
				{
					File::CopyFileOrFolder(originPath, targetPath);
				}
			}

			obj->GetMaterial()->AddTexture(finalTexturePath, texture, type);

			textures[finalTexturePath] = texture;
		}
		else
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::FILE,
				Type::EXCEPTION,
				"Error: Failed to load texture '" + finalTexturePath + "'!\n\n");
		}
		stbi_image_free(data);
	}
}