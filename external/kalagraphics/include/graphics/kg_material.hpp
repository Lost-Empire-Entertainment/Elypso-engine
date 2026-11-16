//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "math_utils.hpp"

#include "core/kg_registry.hpp"

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

namespace KalaGraphics::Graphics
{
    using KalaHeaders::KalaMath::vec4;
    using KalaHeaders::KalaMath::vec3;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::vector;
    using std::default_delete;

    //The two main types of materials for 2D,
    //majority of 2D meshes should use the top one,
    //the bottom one is only for text rendering
    enum class MaterialType2D
    {
        //one base color texture and optional alpha blend/mask
        RECT,
        //one R8 diffuse texture
        FONT
    };

    //The three main types of materials for 3D,
    //each type supports full alpha (opaque, blend, mask) and full RGBA
    enum class MaterialType3D
    {
        //basic material with no shadows and simple shading,
        //one base color texture and optional alpha blend/mask
        UNLIT,

        //artistic lighting approximation.
        //on top of unlit:
        //  material values: specular color and shininess
        //  textures: specular and normal
        BLINN_PHONG,

        //modern PBR (metallic-roughness).
        //on top of unlit:
        //  material values: metallic and roughness
        //  textures: metallic roughness, normal, occlusion and emissive
        PBR
    };

    enum class AlphaMode
    {
        //default opaque mesh
        OPAQUE,
        //makes alpha-based values transparent if transparency is enabled 
        BLEND,
        //fragments below alpha cutoff are discarded, otherwise rendered fully opaque
        MASK
    };

    struct MaterialData_Rect
    {
        vec4 baseColor{ 1.0f };

        AlphaMode alphaMode{};
        f32 alphaCutoff = 0.5f; //clamps from 0.0 to 1.0

        u32 baseColorTextureID{}; //texture material textures slot 0
    };

    struct MaterialData_Font
    {
        vec4 baseColor{ 1.0f };

        u32 baseColorTextureID{}; //texture material textures slot 1
    };

    struct MaterialData_Unlit
    {
        vec4 baseColor{ 1.0f };

        AlphaMode alphaMode{};
        f32 alphaCutoff = 0.5f; //clamps from 0.0 to 1.0

        u32 baseColorTextureID{}; //texture material textures slot 2
    };

    struct MaterialData_BlinnPhong
    {
        vec4 baseColor{ 1.0f };
        vec3 specularColor{ 1.0f };
        f32 shininess = 0.5f; //clamps from 0.0 to 1.0

        AlphaMode alphaMode{};
        f32 alphaCutoff = 0.5f; //clamps from 0.0 to 1.0

        u32 baseColorTextureID{}; //texture material textures slot 3
        u32 specularTextureID{};  //texture material textures slot 4
        u32 normalTextureID{};    //texture material textures slot 5
    };

    struct MaterialData_PBR
    {
        vec4 baseColor{ 1.0f };
        f32 metallic{}; //clamps from 0.0 to 1.0
        f32 roughness = 1.0f; //clamps from 0.0 to 1.0

        //RGB emission multiplier
        vec3 emissiveFactor{};

        AlphaMode alphaMode{};
        f32 alphaCutoff = 0.5f; //clamps from 0.0 to 1.0

        u32 baseColorTextureID{};         //texture material textures slot 6
        u32 metallicRoughnessTextureID{}; //texture material textures slot 7
        u32 normalTextureID{};            //texture material textures slot 8
        u32 occlusionTextureID{};         //texture material textures slot 9
        u32 emissiveTextureID{};          //texture material textures slot 10
    };

    class LIB_API Material
    {
    friend class Viewport;
    friend class Shader;
    friend class Mesh;
    friend class Texture;
    friend struct default_delete<Material>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<Material>& GetRegistry();

        static Material* Initialize(u32 shaderID);

        KNODISCARD
		static bool IsVerboseLoggingEnabled();
        static void SetVerboseLoggingState(bool state);

        u32 GetID() const;
        u32 GetShaderID() const;
        vector<u32> GetMeshIDs() const;

        KNODISCARD
        MaterialType2D GetMaterial2DType() const;
        void SetMaterial2DType(MaterialType2D newValue);

        KNODISCARD
        MaterialType3D GetMaterial3DType() const;
        void SetMaterial3DType(MaterialType3D newValue);

        //
        // SHARED DATA
        //

        KNODISCARD
        const vec4& GetBaseColor() const;
        void SetBaseColor(vec4&& newValue);

        KNODISCARD
        AlphaMode GetAlphaMode() const;
        //If set to A_BLEND or A_MASK then this mesh is filtered separately from opaque models
        //and allows to use .w in color and textures
        void SetAlphaMode(AlphaMode newValue);

        KNODISCARD
        f32 GetAlphaCutoff() const;
        //Set the new alpha cutoff value for mask transparency mode,
        //cannot be used if alpha type is not A_MASK, clamped from 0.0f to 1.0f
        void SetAlphaCutoff(f32 newValue);

        KNODISCARD
        u32 GetBaseColorTextureID() const;
        void SetBaseColorTextureID(u32 newValue);
    private:
        ~Material();

        void Update(VkCommandBuffer buffer);

        void Destroy();

        u32 ID{};
        u32 shaderID{};
        vector<u32> meshIDs{};

        MaterialType2D material2DType{};
        MaterialType3D material3DType{};

        MaterialData_Rect rectData{};
        MaterialData_Font fontData{};

        MaterialData_Unlit unlitData{};
        MaterialData_BlinnPhong blinnPhongData{};
        MaterialData_PBR pbrData{};
    };
}