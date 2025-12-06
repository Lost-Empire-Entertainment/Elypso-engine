//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

namespace KalaGraphics::OpenGL::OpenGL_Shaders
{
	using std::string_view;

	inline constexpr string_view shader_model_vertex = 
	R"(
		#version 330 core

		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec3 aNormal;
		layout (location = 2) in vec2 aTexCoord;
		layout (location = 3) in vec4 aTangent;

		out vec3 vNormal;
		out vec3 vTangent;
		out vec3 vBitangent;
		out vec2 vTexCoord;

		uniform mat4 uModel;
		uniform mat4 uView;
		uniform mat4 uProjection;
		
		void main()
		{
			mat3 normalMatrix = mat3(uModel);
			
			vec3 N = normalize(normalMatrix * aNormal);
			vec3 T = normalize(normalMatrix * aTangent.xyz);
			vec3 B = normalize(cross(N, T) * aTangent.w);
			
			vNormal    = N;
			vTangent   = T;
			vBitangent = B;
			
			vTexCoord = aTexCoord;
			
			gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
		}
	)";

	inline constexpr string_view shader_model_fragment =
	R"(
		#version 330 core

		in vec3 vNormal;
		in vec3 vTangent;
		in vec3 vBitangent;
		in vec2 vTexCoord;
		
		out vec4 FragColor;

		uniform sampler2D uTexture;
		uniform sampler2D uNormalMap;
		uniform bool uUseTexture   = false; //mark as true if you want to pass a texture
		uniform bool uUseNormalMap = false; //mark as true if you want to pass a normalmap

		uniform vec3 uColor;    //blended with texture or non-texture base color
		uniform float uOpacity; //makes this transparent if below 1.0
		
		void main()
		{
			float opacity = clamp(uOpacity, 0.0, 1.0);
			vec3 color = clamp(uColor, 0.0, 1.0);

			if (opacity < 0.001) discard;
			
			vec3 finalColor;
			float finalAlpha;

			if (uUseTexture)
			{
				//tint texture
				
				vec4 texColor = texture(uTexture, vTexCoord);
				finalColor = texColor.rgb * color; 
				finalAlpha = texColor.a * opacity;
			}
			else
			{
				//set color
				
				finalColor = color;
				finalAlpha = opacity;
			}

			FragColor = vec4(finalColor, finalAlpha);
		}
	)";
}