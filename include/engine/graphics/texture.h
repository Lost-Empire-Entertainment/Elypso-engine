#pragma once

#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

#include <string>
#include <vector>

namespace Graphics
{
	class Texture
	{
	public:
		static std::vector<unsigned int> textures;
		const std::string texturePath;
		Texture(const std::string& path);

		void LoadTexture();
	};
}

#endif