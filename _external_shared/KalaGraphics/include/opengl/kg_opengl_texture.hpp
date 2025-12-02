//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <array>
#include <functional>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/kg_registry.hpp"

namespace KalaGraphics::OpenGL
{
	using std::string;
	using std::vector;
	using std::array;
	using std::function;

	using KalaHeaders::vec2;
	
	//Texture internal data type
	enum class OpenGL_TextureType
	{
		Type_2D,
		Type_2DArray,
		Type_3D,
		Type_Cube
	};

	//Texture shape/dimension
	enum class OpenGL_TextureFormat : u8
	{
		Format_Auto, //Auto-assign value (not recommended, can limit usefulness)

		//standard UNORM formats

		Format_R8,
		Format_RG8,
		Format_RGB8,
		Format_RGBA8,
		Format_SRGB8,
		Format_SRGB8A8,

		//float formats

		Format_R16F,
		Format_RG16F,
		Format_RGBA16F,

		Format_R32F,
		Format_RG32F,
		Format_RGBA32F,

		//compressed formats
		//TODO: add BC1-BC7 support

		Format_BC1, //DXT1 - RGB, no alpha
		Format_BC3, //DXT5 - RGBA
		Format_BC4, //Single channel (R)
		Format_BC5, //Two channel (RG)

		//High quality RGBA,
		Format_BC7  
	};
	
	enum class OpenGL_TextureResizeType
	{
		//Use for icons, UI textures.
		//Uses Mitchell filter when shrinking, cubing when enlarging.
		RESIZE_SRGB,

		//Use for normalmaps, heightmaps, scientific data.
		//Should not be used for icons or UI textures.
		//Assumes input is in linear color space (already gamma corrected)
		RESIZE_LINEAR,

		//Use for resizing HDR textures or floating-point buffers.
		//Same as linear but for float* pixel buffers.
		//Texture pixels are internally converted to float and back to u8 when successfully rescaled.
		RESIZE_LINEAR_FLOAT
	};
	
	using KalaGraphics::Core::KalaGraphicsRegistry;

	class LIB_API OpenGL_Texture
	{
	public:
		static inline KalaGraphicsRegistry<OpenGL_Texture> registry{};

		//Load a new texture from an external file.
		//Depth is always clamped to 1 for Type_2D,
		//it is a power of 4 for Type_3D and is clamped internally from 256 to 8192.
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails.
		//Only supports Type_2D and Type_3D.
		static OpenGL_Texture* LoadTexture(
			u32 glID,
			const string& name,
			const string& path,
			OpenGL_TextureType type,
			OpenGL_TextureFormat format,
			bool flipVertically = false,
			u16 depth = 1,
			u8 mipMapLevels = 1);

		//Uses the data from six provided texture files to create a new cubemap texture.
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails. Depth is always 6 for cube map textures.
		//Size for each texture must match and be 1:1 aspect ratio.
		//Only supports Type_Cube
		static OpenGL_Texture* LoadCubeMapTexture(
			u32 glID,
			const string& name,
			const array<string, 6>& texturePaths,
			OpenGL_TextureFormat format,
			bool flipVertically = false,
			u8 mipMapLevels = 1);

		//Uses the data from all the provided texture files to create a new 2D array texture.
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails. Depth is always size of texturePaths vector.
		//Size for each texture must match and be 1:1 aspect ratio.
		//Only supports Type_2DArray
		static OpenGL_Texture* Load2DArrayTexture(
			u32 glID,
			const string& name,
			const vector<string>& texturePaths,
			OpenGL_TextureFormat format,
			bool flipVertically = false,
			u8 mipMapLevels = 1);
			
		inline bool IsInitialized() const { return isInitialized; }

		//Returns the fallback texture,
		//used when a texture fails to load through OpenGL_Texture::LoadTexture
		static OpenGL_Texture* GetFallbackTexture();

		bool Rescale(
			vec2 newSize,
			OpenGL_TextureResizeType type = OpenGL_TextureResizeType::RESIZE_SRGB);

		void HotReload();
		
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
		//Returns the OpenGL context ID of this texture
		inline u32 GetGLID() const { return glID; }
		
		inline vec2 GetSize() const { return size; }
		inline u16 GetDepth() const { return depth; }
		inline u8 GetMipMapLevels() const { return mipMapLevels; }

		inline void SetPixels(const vector<u8>& newPixels) { pixels = newPixels; }
		inline const vector<u8>& GetPixels() const { return pixels; }

		inline void SetCubePixels(const vector<vector<u8>>& newCubePixels)
		{ 
			if (newCubePixels.size() != 6) return;

			cubePixels = newCubePixels;
		}
		inline const vector<vector<u8>>& GetCubePixels() const { return cubePixels; }

		inline void SetLayerPixels(const vector<vector<u8>>& newLayerPixels)
		{
			layerPixels = newLayerPixels;
		}
		inline const vector<vector<u8>>& GetLayerPixels() const { return layerPixels; }

		inline u32 GetTexelCount() const
		{
			return static_cast<u32>(size.x)
				* static_cast<u32>(size.y)
				* static_cast<u32>(depth);
		}

		inline OpenGL_TextureType GetType() const { return type; }
		inline OpenGL_TextureFormat GetFormat() const { return format; }

		//Do not destroy manually, erase from registry instead
		~OpenGL_Texture();
	private:
		//Repeated header and footer of each texture init body with custom data in the middle
		static OpenGL_Texture* TextureBody(
			u32 glID,
			const string& name,
			const vector<string>& texturePaths,
			OpenGL_TextureType type,
			OpenGL_TextureFormat format,
			bool flipVertically,
			u16 depth,
			u8 mipMapLevels,
			const function<bool(
				u32& outTextureID,
				vector<vector<u8>>& outData,
				vec2& outSize,
				OpenGL_TextureFormat& outFormat)>&
			customTextureInitData);
		
		string name{};
		string filePath{};
		
		bool isInitialized{};
		
		u32 ID{};
		u32 textureID{};
		u32 glID{};

		vec2 size{};
		u16 depth = 1;
		u8 mipMapLevels = 1;
		vector<u8> pixels{};              //2D/3D
		vector<vector<u8>> cubePixels{};  //Cubemap
		vector<vector<u8>> layerPixels{}; //2D array

		OpenGL_TextureType type{};
		OpenGL_TextureFormat format{};
	};
}