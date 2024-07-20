//Copyright (c) <2024> <Lost Empire Entertainment>

#version 330 core

uniform float transparency;
uniform vec3 color;
uniform float fadeDistance;
uniform float maxDistance;
uniform vec3 center;
in vec3 fragPos;

out vec4 FragColor;

void main()
{
    float distance = length(fragPos.xz - center.xz);
    float height = length(fragPos.y - center.y);

    if (distance > maxDistance
        || height > 50.0)
    {
        discard;
    }

    float fadeFactor = 1.0;
    if (distance > fadeDistance)
    {
        fadeFactor = 1.0 - (distance - fadeDistance) / (maxDistance - fadeDistance);
        fadeFactor = clamp(fadeFactor, 0.0, 1.0);
    }

    vec4 outputColor = vec4(color, 1.0);
    outputColor.a *= transparency * fadeFactor;

    FragColor = outputColor;
}