//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <vector>
#include <sstream>
#include <algorithm>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

#include "KalaHeaders/log_utils.hpp"

#include "opengl/ku_opengl_texture.hpp"
#include "opengl/ku_opengl_manager.hpp"
#include "opengl/ku_opengl_functions.hpp"
#include "core/ku_core.hpp"

using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaUI::OpenGL::OpenGL_Texture;
using KalaUI::OpenGL::TextureFormat;
using KalaUI::Core::KalaUICore;
using KalaUI::OpenGL::OpenGL_Manager;
using namespace KalaUI::OpenGL::OpenGL_Functions;

using std::string;
using std::string_view;
using std::to_string;
using std::unordered_map;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::path;
using std::filesystem::exists;
using std::vector;
using std::ostringstream;
using std::clamp;
using std::max;
using std::floor;
using std::log2;
using std::array;
using std::transform;
using std::tolower;

constexpr array<string_view, 4> validExtensions =
{
	".png",
	".jpg",
	".jpeg"
};

static u32 TEXTURE_MAX_SIZE{};

static OpenGL_Texture* fallbackTexture{};

constexpr string_view fallbackTextureName = "FallbackTexture";

//The actual fallback texture raw pixel data
static array<u8, 32 * 32 * 4> fallbackPixels{};

//Helper that builds the full checkerboard
static const array<u8, 32 * 32 * 4>& GetFallbackPixels();

static bool CheckTextureData(
	const string& name,
	const string& filePath,
	bool flipVertically,
	TextureFormat format,
	TextureFormat& outformat,
	vec2& outSize,
	vector<u8>& outData,
	int& outNrChannels);

static bool IsValidTexture(
	const string& textureName,
	const string& texturePath);

static bool IsCorrectFormat(
	TextureFormat format,
	int nrChannels);

static bool IsCompressed(TextureFormat f);

static bool IsExtensionSupported(const string& filePath)
{
	string extension = path(filePath).extension().string();

	return find(validExtensions.begin(),
			validExtensions.end(),
			extension)
			!= validExtensions.end();
}

static bool AllTextureExtensionsMatch(
	const vector<string>& filePaths)
{
	string firstExtension = path(filePaths.front()).extension().string();

	for (const auto& filePath : filePaths)
	{
		string thisExtension = path(filePath).extension().string();

		if (thisExtension != firstExtension) return false;
	}

	return true;
}

static u8 GetBytesPerChannel(TextureFormat format);

static string ToLower(string in)
{
	transform(
		in.begin(),
		in.end(),
		in.begin(),
		[](unsigned char c) 
		{ 
			return static_cast<char>(tolower(c)); 
		});

	return in;
}

struct GLFormatInfo {
	GLint internalFormat;
	GLenum format;
	GLenum type;
};

static GLFormatInfo ToGLFormat(TextureFormat fmt);

namespace KalaUI::OpenGL
{
	OpenGL_Texture* OpenGL_Texture::Initialize(
		const uintptr_t* glContext,
		const string& name,
		const string& path,
		TextureFormat format,
		bool flipVertically,
		u8 mipMapLevels)
	{
		return TextureBody(
			glContext,
			name,
			{ path },
			format,
			flipVertically,
			mipMapLevels,
			[&](u32& outTextureID,
				vector<vector<u8>>& outData,
				vec2& outSize,
				TextureFormat& outFormat)
			{
				u32 newTextureID{};
				vector<u8> newData{};
				vec2 newSize{};
				TextureFormat newFormat{};

				int newNrChannels{};

				if (!CheckTextureData(
					name,
					path,
					flipVertically,
					format,
					newFormat,
					newSize,
					newData,
					newNrChannels)) return false;

				//
				// BIND TEXTURE
				//

				glGenTextures(1, &newTextureID);

				GLenum target = GL_TEXTURE_2D;
				GLFormatInfo fmt = ToGLFormat(newFormat);

				glBindTexture(target, newTextureID);

				glTexParameteri(
					target,
					GL_TEXTURE_WRAP_S,
					GL_REPEAT);
				glTexParameteri(
					target,
					GL_TEXTURE_WRAP_T,
					GL_REPEAT);

				if (target == GL_TEXTURE_3D)
				{
					glTexParameteri(
						target,
						GL_TEXTURE_WRAP_R,
						GL_REPEAT);
				}

				//
				// FILTERING
				//

				glTexParameteri(
					target,
					GL_TEXTURE_MIN_FILTER,
					mipMapLevels > 1
					? GL_LINEAR_MIPMAP_LINEAR
					: GL_LINEAR);
				glTexParameteri(
					target,
					GL_TEXTURE_MAG_FILTER,
					GL_LINEAR);

				outTextureID = newTextureID;
				outData = { move(newData) };
				outSize = newSize;
				outFormat = newFormat;

				return true;
			});
	}

	OpenGL_Texture* OpenGL_Texture::GetFallbackTexture()
	{
		if (!fallbackTexture)
		{
			while (glGetError() != GL_NO_ERROR) {} //clear old errors

			unsigned int newTextureID{};
			glGenTextures(1, &newTextureID);

			glBindTexture(GL_TEXTURE_2D, newTextureID);

			//reset state
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexStorage2D(
				GL_TEXTURE_2D,
				1,
				GL_RGBA8,
				32,
				32);

			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				0,
				0,
				32,
				32,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				GetFallbackPixels().data());

			glGenerateMipmap(GL_TEXTURE_2D);

			u32 newID = ++KalaUICore::globalID;
			unique_ptr<OpenGL_Texture> newTexture = make_unique<OpenGL_Texture>();
			OpenGL_Texture* texturePtr = newTexture.get();

			newTexture->name = string(fallbackTextureName);
			newTexture->ID = newID;
			newTexture->size = { 32.0f, 32.0f };
			newTexture->textureID = newTextureID;
			const auto& pixelData = GetFallbackPixels();
			newTexture->pixels.assign(pixelData.begin(), pixelData.end());
			newTexture->format = TextureFormat::Format_RGBA8;

			string errorVal = OpenGL_Manager::GetError();
			if (!errorVal.empty())
			{
				KalaUICore::ForceClose(
					"OpenGL texture error",
					"Failed to load fallback texture! Reason: " + errorVal);

				return nullptr;
			}

			registry.AddContent(newID, move(newTexture));

			fallbackTexture = texturePtr;

			Log::Print(
				"Loaded fallback OpenGL texture with ID '" + to_string(newID) + "'!",
				"OPENGL_TEXTURE",
				LogType::LOG_DEBUG);

			return fallbackTexture;
		}

		return fallbackTexture;
	}

	OpenGL_Texture* OpenGL_Texture::TextureBody(
		const uintptr_t* glContext,
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
			customTextureInitData)
	{
		if (!OpenGL_Manager::IsContextValid(glContext))
		{
			KalaUICore::ForceClose(
				"OpenGL texture error",
				"Failed to load texture '" + name + "' because its gl context was invalid!");

			return nullptr;
		}

		u32 newID = ++KalaUICore::globalID;
		unique_ptr<OpenGL_Texture> newTexture = make_unique<OpenGL_Texture>();
		OpenGL_Texture* texturePtr = newTexture.get();

		Log::Print(
			"Loading texture '" + name + "' with ID '" + to_string(newID) + "'.",
			"OPENGL_TEXTURE",
			LogType::LOG_DEBUG);

		//
		// CALL TEXTURE BODY FUNCTION
		//

		u32 newTextureID{};
		vector<vector<u8>> newData{};
		vec2 newSize{};
		TextureFormat newFormat{};

		if (!customTextureInitData(
			newTextureID,
			newData,
			newSize,
			newFormat))
		{
			return GetFallbackTexture();
		}

		//
		// POST-INIT CONTENT
		//

		texturePtr->name = name;
		texturePtr->ID = newID;
		texturePtr->glContext = glContext;
		
		texturePtr->size = newSize;
		texturePtr->textureID = newTextureID;
		texturePtr->format = newFormat;

		texturePtr->pixels = move(newData[0]);
		texturePtr->filePath = texturePaths[0];

		u32 maxDim = static_cast<u32>(max({ 
			texturePtr->size.x, 
			texturePtr->size.y, 
			1.0f }));

		u8 maxPossibleLevels = 1 + static_cast<u8>(floor(log2(maxDim)));

		if (mipMapLevels > maxPossibleLevels)
		{
			Log::Print(
				"Mipmap levels for texture '" + name + "' was '" + to_string(mipMapLevels) 
				+ "' which is way too high for its resolution. It was reduced to '" 
				+ to_string(maxPossibleLevels) + "' for efficiency.",
				"OPENGL_TEXTURE",
				LogType::LOG_WARNING);
		}

		texturePtr->mipMapLevels = clamp(
			mipMapLevels,
			static_cast<u8>(1),
			maxPossibleLevels);

		string errorVal = OpenGL_Manager::GetError();
		if (!errorVal.empty())
		{
			KalaUICore::ForceClose(
				"OpenGL texture error",
				"Failed to load texture '" + name + "'! Reason: " + errorVal);

			return nullptr;
		}

		{
			GLenum targetType = GL_TEXTURE_2D;
		
			glBindTexture(targetType, newTextureID);

			GLFormatInfo fmt = ToGLFormat(format);

			string target{};

			//allocate all mip levels up front
			glTexStorage2D(
				GL_TEXTURE_2D,
				mipMapLevels,
				fmt.internalFormat,
				static_cast<GLsizei>(newSize.x),
				static_cast<GLsizei>(newSize.y));

			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				0,
				0,
				static_cast<GLsizei>(newSize.x),
				static_cast<GLsizei>(newSize.y),
				fmt.format,
				fmt.type,
				texturePtr->pixels.data());

			if (mipMapLevels > 1) glGenerateMipmap(targetType);

			string errorVal = OpenGL_Manager::GetError();
			if (!errorVal.empty())
			{
				KalaUICore::ForceClose(
					"OpenGL texture error",
					"Failed to load texture '" + name + "' with " + target + "! Reason: " + errorVal);

				return nullptr;
			}
		}

		texturePtr->isInitialized = true;

		registry.AddContent(newID, move(newTexture));

		Log::Print(
			"Loaded OpenGL texture '" + name + "' with ID '" + to_string(newID) + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_SUCCESS);

		return texturePtr;
	}

	OpenGL_Texture::~OpenGL_Texture()
	{
		Log::Print(
			"Destroying texture '" + name + "' with ID '" + to_string(ID) + "'.",
			"OPENGL_TEXTURE",
			LogType::LOG_INFO);

		if (textureID != 0)
		{
			glDeleteTextures(1, &textureID);
			textureID = 0;
		}
	}
}

const array<u8, 32 * 32 * 4>& GetFallbackPixels()
{
	if (fallbackPixels[0] != 0) return fallbackPixels;

	constexpr int texSize = 32;
	constexpr int blockSize = 8;

	for (int y = 0; y < texSize; ++y)
	{
		for (int x = 0; x < texSize; ++x)
		{
			//determine block color - alternate every 8 pixels
			bool isMagenta = (((x / blockSize) + (y / blockSize)) % 2 == 0);

			int idx = (y * texSize + x) * 4;
			using index_t = array<u8, 32 * 32 * 4>::size_type;

			auto Push = [idx](u8 adder, u8 result)
				{
					fallbackPixels[static_cast<index_t>(idx) + adder] = result;
				};

			Push(0, isMagenta ? 255 : 0); //R
			Push(1, 0);                   //G
			Push(2, isMagenta ? 255 : 0); //B
			Push(3, 255);                 //A
		}
	}

	return fallbackPixels;
}

bool CheckTextureData(
	const string& name,
	const string& filePath,
	bool flipVertically,
	TextureFormat format,
	TextureFormat& outformat,
	vec2& outSize,
	vector<u8>& outData,
	int& outNrChannels)
{
	TextureFormat newFormat{};
	vector<u8> newData{};
	int newNrChannels{};

	if (name == fallbackTextureName)
	{
		Log::Print(
			"Texture name '" + name + "' is reserved for the fallback texture!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	if (!IsValidTexture(name, filePath)) return false;

	//
	// GET TEXTURE DATA FROM FILE
	//

	int width{};
	int height{};
	stbi_set_flip_vertically_on_load(flipVertically);

	unsigned char* stbiData = stbi_load(
		(filePath).c_str(),
		&width,
		&height,
		&newNrChannels,
		0);

	if (!stbiData)
	{
		Log::Print(
			"Failed to get texture data from texture path '" + filePath + "' for texture '" + name + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	newFormat = format;
	if (newFormat == TextureFormat::Format_Auto)
	{
		ostringstream oss{};
		oss << "Texture format 'Format_Auto' was used for texture '" + name + "'."
			<< " This is not recommended, consider using true format.";

		Log::Print(
			oss.str(),
			"OPENGL_TEXTURE",
			LogType::LOG_WARNING);

		if (newNrChannels == 1) newFormat = TextureFormat::Format_R8;
		else if (newNrChannels == 2) newFormat = TextureFormat::Format_RG8;
		else if (newNrChannels == 3) newFormat = TextureFormat::Format_RGB8;
		else if (newNrChannels == 4) newFormat = TextureFormat::Format_RGBA8;
		else
		{
			Log::Print(
				"Unsupported channel count '" + to_string(newNrChannels) + "' for texture '" + name + "'!",
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			stbi_image_free(stbiData);

			return false;
		}
	}
	else
	{
		if (!IsCorrectFormat(
			newFormat,
			newNrChannels))
		{
			Log::Print(
				"Texture '" + name + "' was loaded with an incorrect format! Channel count is '" + to_string(newNrChannels) + "'",
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			stbi_image_free(stbiData);

			return false;
		}
	}

	u8 bpc = GetBytesPerChannel(newFormat);
	if (bpc == 0)
	{
		stbi_image_free(stbiData);

		return false;
	}

	size_t dataSize = static_cast<size_t>(width) * height * newNrChannels * bpc;
	newData.assign(stbiData, stbiData + dataSize);

	stbi_image_free(stbiData);

	string widthStr = to_string(width);
	string heightStr = to_string(height);

	if (width <= 0
		|| height <= 0)
	{
		ostringstream oss{};
		oss << "failed to load texture '" << name
			<< "' because the texture size '" << widthStr << "x" << heightStr
			<< "' is too small! Size cannot be '0x0' pixels or below!";

		Log::Print(
			oss.str(),
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	//clamp to gpu texture resolution upper bound
	if (TEXTURE_MAX_SIZE == 0)
	{
		GLint maxSize{};
		glGetIntegerv(
			GL_MAX_TEXTURE_SIZE,
			&maxSize);

		TEXTURE_MAX_SIZE = maxSize;
	}

	if (width > TEXTURE_MAX_SIZE
		|| height > TEXTURE_MAX_SIZE)
	{
		string maxSizeStr = to_string(TEXTURE_MAX_SIZE);

		ostringstream oss{};
		oss << "failed to load texture '" << name
			<< "' because the texture size '" << widthStr << "x" << heightStr
			<< "' is too big! Size cannot be above '" 
			<< maxSizeStr << "x" << maxSizeStr << "' pixels!";

		Log::Print(
			oss.str(),
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	outformat = newFormat;
	outSize = vec2(width, height);
	outData = newData;
	outNrChannels = newNrChannels;

	return true;
}

bool IsValidTexture(
	const string& textureName,
	const string& texturePath)
{
	//texture name must not be empty
	if (textureName.empty()
		|| textureName.length() > 50)
	{
		Log::Print(
			"Texture name cannot be empty or longer than 50 characters!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	//texture path must not be empty
	if (texturePath.empty())
	{
		Log::Print(
			"Cannot load texture '" + textureName + "' because its path is empty!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	//texture file must exist
	if (!exists(texturePath))
	{
		Log::Print(
			"Cannot load texture '" + textureName + "' because its path '" + texturePath + "' does not exist!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	string lowerFilePath = ToLower(texturePath);

	//texture file must have an extension
	if (!path(lowerFilePath).has_extension())
	{
		Log::Print(
			"Cannot load texture '" + textureName + "' because it has no extension!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	//texture file must have a supported extension
	if (!IsExtensionSupported(lowerFilePath))
	{
		Log::Print(
			"Cannot load texture '" + textureName + "' because its extension '" + path(lowerFilePath).extension().string() + "' is not supported!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	return true;
}

bool IsCorrectFormat(
	TextureFormat format,
	int nrChannels)
{
	switch (nrChannels)
	{
	case 1: return format == TextureFormat::Format_R8;
	case 2: return format == TextureFormat::Format_RG8;
	case 3: return format == TextureFormat::Format_RGB8;
	case 4: return format == TextureFormat::Format_RGBA8;
	default: return false;
	}
}

u8 GetBytesPerChannel(TextureFormat format)
{
	switch (format)
	{
	case TextureFormat::Format_R8:
	case TextureFormat::Format_RG8:
	case TextureFormat::Format_RGB8:
	case TextureFormat::Format_RGBA8:
		return 1;
	default:
		Log::Print(
			"GetBytesPerChannel called on invalid format! Compressed formats don't have per-channel bytes.",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return 0;
	}
}

GLFormatInfo ToGLFormat(TextureFormat fmt)
{
	switch (fmt)
	{
	//
	// UNORM
	//

	case TextureFormat::Format_R8:       return { GL_R8,    GL_RED,  GL_UNSIGNED_BYTE };
	case TextureFormat::Format_RG8:      return { GL_RG8,   GL_RG,   GL_UNSIGNED_BYTE };
	case TextureFormat::Format_RGB8:     return { GL_RGB8,  GL_RGB,  GL_UNSIGNED_BYTE };
	case TextureFormat::Format_RGBA8:    return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };

	default: return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE }; //safe fallback
	}
}