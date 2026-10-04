#version 460

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inVertexColor;
layout(location = 2) in vec4 inColor;
layout(location = 3) flat in uint alphaMode;
layout(location = 4) flat in float alphaCutoff;

layout(set = 2, binding = 0) uniform sampler2D uTexture;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 baseColor = texture(
        uTexture, inUV)
        * inVertexColor
        * inColor;

    //A_OPAQUE
    if (alphaMode == 0) baseColor.a = 1.0;
    //A_MASK
    else if (alphaMode == 2)
    {
        if (baseColor.a < alphaCutoff) discard;

        baseColor.a = 1.0;
    }

    outColor = vec4(
        baseColor.rgb * baseColor.a,
        baseColor.a);
}
