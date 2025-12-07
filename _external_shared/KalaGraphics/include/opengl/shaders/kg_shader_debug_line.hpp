//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

namespace KalaGraphics::OpenGL::OpenGL_Shaders
{
	//Use this one for drawing line-based debug shapes
	
	using std::string_view;
	
	inline constexpr string_view shader_debug_line_vertex = 
	R"(
		#version 330 core
		
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in float aDist;
		
		out float vDist;
		
		uniform mat4 uModel;
		uniform mat4 uView;
		uniform mat4 uProjection;
		uniform float uThickness; //pixel width
		
		void main()
		{
			vDist = aDist;
			
			gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
		}
	)";
	
	inline constexpr string_view shader_debug_line_fragment = 
	R"(
		#version 330 core
		
		in float vDist;
		out vec4 FragColor;
		
		uniform float uOpacity;     //makes the model transparent if below 1.0
		uniform vec3 uDiffuseColor; //line color
		uniform float uThickness;   //line width in pixels
		uniform float uFadeSize;    //smooth fade zone width
		
		void main()
		{
			float opacity = clamp(uOpacity, 0.0, 1.0);
			vec3 color = clamp(uDiffuseColor, 0.0, 1.0);

			//skip if too transparent
			if (opacity < 0.001) discard;
			
			float halfWidth = uThickness * 0.5f;
			
			//scale vDist into actual width space
			float dist = abs(vDist) * halfWidth;
			
			//skip if outside the band
			if (dist > halfWidth) discard;
			
			//smooth fade zone
			float edge = halfWidth - uFadeSize;
			
			//1.0 if distance <= edge, fades to 0.0 near halfWidth
			float smoothAlpha = smoothstep(halfWidth, edge, dist);
			
			FragColor = vec4(color, opacity * smoothAlpha);
		}
	)";
}