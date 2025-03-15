//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <map>
#include <vector>

//engine
#include "component.hpp"
#include "shader.hpp"

namespace Graphics::Components
{
    using std::string;
    using std::map;
    using std::vector;

    using Graphics::Shader;

    class MaterialComponent : public Component
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

        MaterialComponent(
            bool isTransparent = false,
            float transparentValue = 1.0f,
            float shininessValue = 0.5f) :
            isTransparent(isTransparent),
            transparentValue(transparentValue),
            shininessValue(shininessValue) {}

        void AddTexture(
            const string& textureName, 
            const unsigned int& textureID, 
            const TextureType& textureType)
        {
            auto it = textures.find(textureType);
            if (it != textures.end())
            {
                textures.erase(it);
            }
            textures[textureType][textureName] = textureID;
        }

        void AddShader(
            const string& vertShader, 
            const string& fragShader, 
            const Shader& newShader)
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

        type_index GetType() const override { return typeid(MaterialComponent); }

        void SetShininessValue(float newShininessValue) { shininessValue = newShininessValue; }
        void SetTransparent(bool newTransparent) { isTransparent = newTransparent; }
        void SetTransparentValue(float newTransparentValue) { transparentValue = newTransparentValue; }

        const float& GetShininessValue() const { return shininessValue; }
        const bool& IsTransparent() const { return isTransparent; }
        const float& GetTransparentValue() const { return transparentValue; }
    private:
        bool isTransparent;
        float shininessValue;
        float transparentValue;
        map<TextureType, map<string, unsigned int>> textures;
        vector<string> shaderNames;
        Shader shader;
    };
}