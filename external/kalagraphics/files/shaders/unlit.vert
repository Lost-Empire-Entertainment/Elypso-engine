#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;

layout(set = 0, binding = 0) uniform CameraUBO
{
    mat4 projectionMatrix;
} camera;

layout(set = 1, binding = 0) uniform Mesh
{
    mat4 modelMatrix;
} meshData;

layout(push_constant) uniform PushConstants
{
    vec4 color;
    uint alphaMode;
    float alphaCutoff;
} pushData;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec4 outVertexColor;
layout(location = 3) out vec4 outColor;
layout(location = 4) flat out uint outAlphaMode;
layout(location = 5) flat out float outAlphaCutoff;

void main()
{
    gl_Position =
        camera.projectionMatrix
        * meshData.modelMatrix
        * vec4(inPosition, 1.0);

    outNormal = normalize(
        transpose(inverse(mat3(meshData.modelMatrix)))
        * inNormal);

    outUV = inUV;
    outVertexColor = inColor;
    outColor = pushData.color;
    outAlphaMode = pushData.alphaMode;
    outAlphaCutoff = pushData.alphaCutoff;
}
