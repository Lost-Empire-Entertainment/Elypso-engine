//Copyright (c) <2025> <Lost Empire Entertainment>

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define MAX_SPOT_SHADOWS 16
out vec4 FragPosLightSpace[MAX_SPOT_SHADOWS];
uniform mat4 spotLightSpaceMatrices[MAX_SPOT_SHADOWS];

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
	
	for (int i = 0; i < MAX_SPOT_SHADOWS; ++i)
	{
		FragPosLightSpace[i] = spotLightSpaceMatrices[i] * vec4(FragPos, 1.0);
	}
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}