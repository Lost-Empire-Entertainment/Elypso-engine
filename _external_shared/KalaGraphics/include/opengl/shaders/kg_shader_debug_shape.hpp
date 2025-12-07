//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

namespace KalaGraphics::OpenGL::OpenGL_Shaders
{
	//Use this one for drawing any debug shapes, unlike kg_shader_debug_line.hpp
	
	using std::string_view;
	
	inline constexpr string_view shader_debug_shape_vertex = 
	R"(
		#version 330 core
		
		layout (location = 0) in vec3 aPos;
		
		uniform mat4 uModel;
		uniform mat4 uView;
		uniform mat4 uProjection;
		
		void main()
		{
			gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
		}
	)";
	
	inline constexpr string_view shader_debug_shape_fragment = 
	R"(
		#version 330 core
		
		out vec4 FragColor;
		
		uniform float uOpacity;     //makes the model transparent if below 1.0
		uniform vec3 uDiffuseColor; //base color of the model
		
		void main()
		{
			float opacity = clamp(uOpacity, 0.0, 1.0);
			vec3 color = clamp(uDiffuseColor, 0.0, 1.0);

			if (opacity < 0.001) discard;
			
			FragColor = vec4(color, opacity);
		}
	)";
}