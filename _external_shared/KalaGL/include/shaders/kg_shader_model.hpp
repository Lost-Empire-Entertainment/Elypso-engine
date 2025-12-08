//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

namespace KalaGL::OpenGL_Shaders
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
		
		out vec3 vFragPos;

		uniform mat4 uModel;
		uniform mat4 uView;
		uniform mat4 uProjection;
		
		void main()
		{
			vec4 worldPos = uModel * vec4(aPos, 1.0);
			vFragPos = worldPos.xyz;
			
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

		in vec2 vTexCoord;
		
		in vec3 vNormal;
		in vec3 vTangent;
		in vec3 vBitangent;
		
		in vec3 vFragPos;
		
		out vec4 FragColor;
		
		//
		// MATERIAL
		//
		
		uniform vec3 uViewPos;    //camera position
		uniform float uShininess; //affects how much light something reflects
		uniform float uOpacity;   //makes the model transparent if below 1.0
		uniform bool uTwoSided;   //set to true for flat models and planes
		
		uniform vec3 uDiffuseColor; //base color of the model
		uniform bool uHasDiffuseTex;
		uniform sampler2D uDiffuseTex;
		
		uniform bool uHasNormalTex;
		uniform sampler2D uNormalTex;
		
		uniform vec3 uSpecularColor;
		uniform bool uHasSpecularTex;
		uniform sampler2D uSpecularTex;
		
		uniform vec3 uEmissiveColor;
		uniform bool uHasEmissiveTex;
		uniform sampler2D uEmissiveTex;
	
		//
		// POINT LIGHT
		//
		
		#define MAX_PL_COUNT 128
		
		struct PointLight
		{
			vec4 position;
			
			int canRender;
			float intensity;
			float maxRange;
			float _pad1;
			
			vec4 color;
			
			float constant;
			float linear;
			float quadratic;
			float _pad2;
			
			//+ canCastShadows in z, w unused
			vec4 shadowResolution;
			
			float shadowStrength;
			float filterRadius;
			
			float bias;
			float slopeBias;
			
			float nearPlane;
			float farPlane;
			float _pad3[2];
		};
		
		uniform PointLightBlock
		{
			PointLight uPointLights[MAX_PL_COUNT];
		};
		uniform int uPointLightCount;
		
		vec3 ComputePointLight(
			vec3 baseColor,
			vec3 specularMap,
			vec3 worldNormal,
			vec3 viewDir,
			vec3 vFragPos,
			int i);
		
		void main()
		{
			//
			// BASE COLOR
			//
			
			float opacity = clamp(uOpacity, 0.0, 1.0);
			vec3 color = clamp(uDiffuseColor, 0.0, 1.0);

			if (opacity < 0.001) discard;
			
			vec3 baseColor;
			float alpha;

			if (uHasDiffuseTex)
			{
				//tint texture
				
				vec4 texColor = texture(uDiffuseTex, vTexCoord);
				baseColor = texColor.rgb * color; 
				alpha = texColor.a * opacity;
			}
			else
			{
				//set color
				
				baseColor = color.rgb;
				alpha = opacity;
			}
			
			//
			// NORMALS
			//
			
			vec3 worldNormal = normalize(vNormal);
			
			if (uHasNormalTex)
			{
				vec3 tangentNormal = texture(uNormalTex, vTexCoord).xyz;
				
				//convert to correct range
				tangentNormal = tangentNormal * 2.0 - 1.0;
				
				//build TBN matrix
				mat3 TBN = mat3(vTangent, vBitangent, vNormal);
				
				//transform to world space
				worldNormal = normalize(TBN * tangentNormal);
			}
			
			//
			// SPECULAR MAPS
			//
			
			vec3 specularMap = vec3(1.0);
			
			if (uHasSpecularTex)
			{
				specularMap = texture(uSpecularTex, vTexCoord).xyz;
			}
			
			//
			// EMISSIVE AND AMBIENT
			//
			
			vec3 ambient = baseColor * 0.01;
			
			vec3 emissive = vec3(0.0);
			if (uHasEmissiveTex) emissive = texture(uEmissiveTex, vTexCoord).rgb;
			else emissive = uEmissiveColor;
			
			vec3 result = ambient + emissive;
			
			//
			// POINT LIGHTS
			//
			
			//direction from fragment to camera,
			//used for specular highlights, reflection and shadow bias
			vec3 viewDir = normalize(uViewPos - vFragPos);
			
			for (int i = 0; i < uPointLightCount; i++)
			{					
				result += ComputePointLight(
					baseColor,
					specularMap,
					worldNormal,
					viewDir,
					vFragPos,
					i);
			}
		
			//
			// END RESULT
			//

			FragColor = vec4(result, alpha);
		}
		
		vec3 ComputePointLight(
			vec3 baseColor,
			vec3 specularMap,
			vec3 worldNormal,
			vec3 viewDir,
			vec3 vFragPos,
			int i)
		{
			//future ideas to consider:
			//  - fake Fresnel rim lighting
			//  - tone-mapped specular highlights
			//  - two-sided lighting for planes (uTwoSided)
			//  - soft shadow injection per-light
			//  - energy conversation clamp
			//  - highlight softening for low shininess
			
			//skip if disabled
			if (uPointLights[i].canRender == 0) return vec3(0.0);
			
			vec3 toLight = uPointLights[i].position.xyz - vFragPos;
			
			//skip if too far to render meaningfully
			float distance = length(toLight);
			if (distance > uPointLights[i].maxRange) return vec3(0.0);
			
			vec3 lightDir = normalize(toLight);
			
			//
			// DIFFUSE
			//
			
			float diff = max(dot(worldNormal, lightDir), 0.0);
			vec3 diffuse = diff * baseColor * uPointLights[i].color.rgb;
			
			//
			// SPECULAR
			//
			
			vec3 halfDir = normalize(lightDir + viewDir);
				
			float spec = pow(max(dot(worldNormal, halfDir), 0.0), uShininess);
			spec = max(spec, 1e-4);
			
			vec3 specular = 
				spec
				* specularMap
				* uSpecularColor
				* uPointLights[i].color.rgb;
			
			//
			// ATTENUATION
			//
			
			float attenuation = 1.0 / (
				uPointLights[i].constant
				+ uPointLights[i].linear * distance
				+ uPointLights[i].quadratic * (distance * distance));
				
			attenuation = max(attenuation, 1e-4);
			
			//fade near max range
			float fade = 1.0 - smoothstep(
				uPointLights[i].maxRange * 0.5,
				uPointLights[i].maxRange,
				distance);
				
			fade = max(fade, 0.01);
			
			attenuation *= fade;
			
			//
			// RESULT
			//
				
			return (diffuse + specular) * attenuation * uPointLights[i].intensity;
		}
	)";
}