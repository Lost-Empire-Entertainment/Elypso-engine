//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <functional>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/ku_registry.hpp"

namespace KalaUI::OpenGL
{
	using std::string;
	using std::vector;
	using std::function;

	using KalaHeaders::KalaMath::vec2;

	//Texture shape/dimension
	enum class TextureFormat : u8
	{
		Format_Auto  = 0,

		Format_R8    = 1,
		Format_RG8   = 2,
		Format_RGB8  = 3,
		Format_RGBA8 = 4
	};
	
	using KalaUI::Core::KalaUIRegistry;

	class LIB_API OpenGL_Texture
	{
	public:
		static inline KalaUIRegistry<OpenGL_Texture> registry{};

		//Load a new texture from an external file.
		//Depth is always clamped to 1 for Type_2D,
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails.
		static OpenGL_Texture* Initialize(
			uintptr_t glContext,
			const string& name,
			const string& path,
			TextureFormat format = TextureFormat::Format_Auto,
			bool flipVertically = false,
			u8 mipMapLevels = 1);
			
		inline bool IsInitialized() const { return isInitialized; }

		//Returns the fallback texture,
		//used when a texture fails to load through OpenGL_Texture::LoadTexture
		static OpenGL_Texture* GetFallbackTexture();
		
		inline void SetName(const string& newName)
		{
			//skip if name is empty, same as existing or too long
			if (newName.empty()
				|| newName == name
				|| newName.length() > 50) return;

			name = newName;
		}
		inline const string& GetName() const { return name; }

		inline const string& GetPath() const { return filePath; }

		//Returns the global ID of this shader
		inline u32 GetID() const { return ID; }
		//Returns the OpenGL texture ID of this texture
		inline u32 GetTextureID() const { return textureID; }
		
		//Returns the OpenGL context of this texture
		inline uintptr_t GetGLContext() const { return glContext; }
		
		inline vec2 GetSize() const { return size; }
		inline u8 GetMipMapLevels() const { return mipMapLevels; }

		inline const vector<u8>& GetPixels() const { return pixels; }

		inline u32 GetTexelCount() const
		{
			return static_cast<u32>(size.x)
				* static_cast<u32>(size.y)
				* static_cast<u32>(1);
		}

		inline TextureFormat GetFormat() const { return format; }

		//Do not destroy manually, erase from registry instead
		~OpenGL_Texture();
	private:
		static OpenGL_Texture* TextureBody(
			uintptr_t glContext,
			const string& name,
			const vector<string>& texturePaths,
			TextureFormat format,
			bool flipVertically,
			u8 mipMapLevels,
			const function<bool(
				u32& outTextureID,
				vector<vector<u8>>& outData,
				vec2& outSize,
				TextureFormat& outFormat)>&
			customTextureInitData);
	
		string name{};
		string filePath{};
		
		bool isInitialized{};
		
		u32 ID{};
		u32 textureID{};
		uintptr_t glContext{};

		vec2 size{};
		u8 mipMapLevels = 1;
		vector<u8> pixels{};

		TextureFormat format{};
	};
}