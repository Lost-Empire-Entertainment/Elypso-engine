#version 460

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

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

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outVertexColor;
layout(location = 2) out vec4 outColor;
layout(location = 3) flat out uint outAlphaMode;
layout(location = 4) flat out float outAlphaCutoff;

void main()
{
    gl_Position =
        camera.projectionMatrix
        * meshData.modelMatrix
        * vec4(inPosition, 0.0, 1.0);

    outUV = inUV;
    outVertexColor = inColor;
    outColor = pushData.color;
    outAlphaMode = pushData.alphaMode;
    outAlphaCutoff = pushData.alphaCutoff;
}
