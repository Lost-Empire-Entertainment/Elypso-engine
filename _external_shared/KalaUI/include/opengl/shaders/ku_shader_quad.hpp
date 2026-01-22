//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

namespace KalaUI::OpenGL::OpenGL_Shaders
{
	using std::string_view;

	inline constexpr string_view shader_quad_vertex = 
	R"(
		#version 330 core

		layout (location = 0) in vec2 aPos;
		layout (location = 1) in vec2 aTexCoord;

		out vec2 vTexCoord;

		uniform mat4 uModel;
		uniform mat4 uProjection;
		
		void main()
		{
			//view matrix is identity and unused

			vec4 worldPos = uProjection * uModel * vec4(aPos, 0.0, 1.0);
			gl_Position = vec4(worldPos);

			vTexCoord = aTexCoord;
		}
	)";

	inline constexpr string_view shader_quad_fragment =
	R"(
		#version 330 core

		in vec2 vTexCoord;
		
		out vec4 FragColor;
	
		uniform sampler2D uTexture;
		uniform bool uUseTexture = false; //mark as true if you want to pass a texture

		uniform vec3 uColor;    //blended with texture or non-texture base color
		uniform float uOpacity; //makes this transparent if below 1.0
		
		void main()
		{
			float opacity = clamp(uOpacity, 0.0, 1.0);

			if (opacity < 0.001) discard;
			
			vec3 finalColor;
			float finalAlpha;

			if (uUseTexture)
			{
				//tint texture
				
				vec4 texColor = texture(uTexture, vTexCoord);
				finalColor = texColor.rgb * uColor; 
				finalAlpha = texColor.a * opacity;
			}
			else
			{
				//set color
				
				finalColor = uColor;
				finalAlpha = opacity;
			}

			FragColor = vec4(finalColor, finalAlpha);
		}
	)";
}