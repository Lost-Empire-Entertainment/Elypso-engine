#pragma once

#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

#include <string>

namespace Graphics
{
	class Texture
	{
	public:
		static unsigned int texture;
		const std::string texturePath;
		Texture(const std::string& path);

		void LoadTexture();
	};
}

#endif