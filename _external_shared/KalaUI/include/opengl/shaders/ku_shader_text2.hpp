//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

namespace KalaUI::OpenGL::OpenGL_Shaders
{
	using std::string_view;

	inline constexpr string_view shader_text_vertex2 = 
	R"(
		#version 330 core

		layout (location = 0) in vec2 aPos;
		layout (location = 1) in vec2 aTexCoord;
		layout (location = 2) in int aTexIndex; //texture array layer index

		out vec2 vTexCoord;
		flat out int vTexIndex;

		uniform mat4 uModel;
		uniform mat4 uProjection;
		
		void main()
		{
			//view matrix is identity and unused

			vec4 worldPos = uProjection * uModel * vec4(aPos, 0.0, 1.0);
			gl_Position = vec4(worldPos);

			vTexCoord = aTexCoord;
			vTexIndex = aTexIndex;
		}
	)";

	inline constexpr string_view shader_text_fragment2 =
	R"(
		#version 330 core

		in vec2 vTexCoord;
		flat in int vTexIndex;
		
		out vec4 FragColor;
	
		uniform sampler2DArray uTextures;
		uniform int uLayerCount;

		uniform vec3 uColor;    //blended with texture base color
		uniform float uOpacity; //makes this transparent if below 1.0
		
		void main()
		{
			float opacity = clamp(uOpacity, 0.0, 1.0);

			if (opacity < 0.001) discard;

			if (vTexIndex < 0 || vTexIndex >= uLayerCount) discard;
			
			//treat red as alpha
			float coverage = texture(uTextures, vec3(vTexCoord, vTexIndex)).r;
			float alpha = coverage * opacity;

			if (alpha < 0.001) discard;

			FragColor = vec4(uColor, alpha);
		}
	)";
}