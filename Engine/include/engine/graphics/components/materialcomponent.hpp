//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "gameobject.hpp"
#include "shader.hpp"
#include "core.hpp"

namespace Graphics::Components
{
	using std::map;
	using std::vector;
	using std::string;
	using std::cout;

	using Graphics::Shader;
	using Graphics::Shape::Component;
	using Core::Engine;

	class Material : public Component
	{
	public:
		enum class TextureType
		{
			diffuse,
			specular,
			normal,
			height,
			misc_icon_blank,
			misc_icon_move,
			misc_icon_rotate,
			misc_icon_scale
		};

		Material() {}

		void Initialize(const string& vertShaderPath, const string& fragShaderPath)
		{
			auto par = parent.lock();
			cout << "initialized material for " << par->GetName() << "\n";

			Shader newShader = Shader::LoadShader(vertShaderPath, fragShaderPath);
			AddShader(vertShaderPath, fragShaderPath, newShader);
		}

		void AddTexture(const string& textureName, const unsigned int& textureID, const TextureType& textureType)
		{
			auto it = textures.find(textureType);
			if (it != textures.end())
			{
				textures.erase(it);
			}

			textures[textureType][textureName] = textureID;
		}

		void AddShader(const string& vertShader, const string& fragShader, const Shader& newShader)
		{
			shaderNames.push_back(vertShader);
			shaderNames.push_back(fragShader);
			shader = newShader;
		}

		const unsigned int& GetTextureID(const TextureType& type) const
		{
			static unsigned int none = 0;

			auto it = textures.find(type);
			if (it != textures.end())
			{
				const auto& textureMap = it->second;
				if (!textureMap.empty())
				{
					return textureMap.begin()->second;
				}
			}
			return none;
		}
		const string& GetTextureName(const TextureType& type) const
		{
			static string none = "";

			auto it = textures.find(type);
			if (it != textures.end())
			{
				const auto& textureMap = it->second;
				if (!textureMap.empty())
				{
					return textureMap.begin()->first;
				}
			}
			return none;
		}
		unsigned int GetTextureCount() const
		{
			unsigned int totalCount = 0;
			for (const auto& pair : textures)
			{
				const auto& textureMap = pair.second;
				totalCount += static_cast<unsigned int>(textureMap.size());
			}
			return totalCount;
		}
		bool TextureExists(const string& texturePath) const
		{
			for (const auto& pair : textures)
			{
				const auto& textureMap = pair.second;
				for (const auto& texture : textureMap)
				{
					if (texture.first == texturePath)
					{
						return true;
					}
				}
			}
			return false;
		}

		const string& GetShaderName(unsigned int position) const
		{
			return shaderNames[position];
		}
		const Shader& GetShader() const
		{
			return shader;
		}
	private:
		map<TextureType, map<string, unsigned int>> textures;
		vector<string> shaderNames;
		Shader shader;
	};
}