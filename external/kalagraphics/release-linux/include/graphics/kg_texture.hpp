//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <array>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kg_registry.hpp"

struct VkBuffer_T;
using VkBuffer = VkBuffer_T*;

struct VmaAllocation_T;
using VmaAllocation = VmaAllocation_T*;

struct VkDescriptorSet_T;
using VkDescriptorSet = VkDescriptorSet_T*;

struct VkSampler_T;
using VkSampler = VkSampler_T*;

struct VkImage_T;
using VkImage = VkImage_T*;

struct VkImageView_T;
using VkImageView = VkImageView_T*;

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

namespace KalaGraphics::PrimitiveWidgets
{
    class Text;
}

namespace KalaGraphics::CompositeWidgets
{
    class Button;
}

namespace KalaGraphics::Graphics
{
    using KalaHeaders::KalaMath::vec2;
    using KalaHeaders::KalaMath::vec4;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::vector;
    using std::array;
    using std::pair;
    using std::default_delete;

    enum class TexturePixelFormat : u8
    {
        FORMAT_BASIC_R8       = 0, //1 channel,  8-bit UNORM
        FORMAT_BASIC_R8G8     = 1, //2 channels, 8-bit UNORM
        FORMAT_BASIC_R8G8B8A8 = 3, //4 channels, 8-bit UNORM

        FORMAT_SRGB_R8G8B8A8  = 4  //4 channels, 8-bit sRGB-encoded

        //TODO: consider if HDR is worth adding or not
    };

    enum class TextureType : u8
    {
        TYPE_2D            = 0, //single, flat image, layer count always 1
        TYPE_2D_ARRAY      = 1, //N independent 2D layers
        TYPE_CUBEMAP       = 2, //always 6 layers, one per cube face
        TYPE_CUBEMAP_ARRAY = 3, //6 layers per cubemap, one per cube face
        TYPE_3D            = 4  //volumetric, layerCount = depth
    };

    enum class TextureFilterMode : u8
    {
        FILTER_LINEAR = 0, //standard blended filter mode, good for realistic textures
        FILTER_NEAREST = 1 //unfiltered texture, good for pixel games
    };

    enum class TextureShadowMapMode : u8
    {
        //comparison always passes,
        //default if not using shadow mapping
        MODE_ALWAYS = 0,

        //standard shadow map comparison,
        //passes if the fragments depth is less
        //than the stored shadow map depth
        MODE_LESS = 1,
        //same as less, but also passes on exact equality,
        //sometimes used to avoid self-shadowing artifacts
        //at the exact depth boundary
        MODE_LESS_OR_EQUAL = 2
    };

    enum class TextureWrapMode : u8
    {
        WRAP_REPEAT = 0,
        WRAP_MIRRORED_REPEAT = 1,
        WRAP_CLAMP_TO_EDGE = 2,
        WRAP_CLAMP_TO_BORDER = 3
    };

    enum class TextureBorderColor : u8
    {
        COLOR_TRANSPARENT_BLACK = 0,
        COLOR_OPAQUE_BLACK = 1,
        COLOR_OPAQUE_WHITE = 2
    };

    //Defaults to 1x1 white pixel unless overwritten
    struct TextureData
    {
        //default pixel data is always a 1x1 white pixel
        vector<u8> pixelData = 
        { 
            0xFF, 
            0xFF, 
            0xFF, 
            0xFF
        };

        TexturePixelFormat format       = TexturePixelFormat::FORMAT_BASIC_R8G8B8A8;
        TextureType type                = TextureType::TYPE_2D;
        TextureFilterMode filterMode    = TextureFilterMode::FILTER_LINEAR;
        TextureShadowMapMode shadowMode = TextureShadowMapMode::MODE_ALWAYS;
        TextureWrapMode wrapMode        = TextureWrapMode::WRAP_REPEAT;
        //only used if wrap mode is WRAP_CLAMP_TO_BORDER
        TextureBorderColor borderColor  = TextureBorderColor::COLOR_OPAQUE_BLACK;

        //disabled by default, quality improvement for textures in 3D spaces when viewed at steep angles,
        //should not be used for UI textures or if using FILTER_NEAREST
        bool useAnisotropy{};

        vec2 size       = 1; //width and height
        u16 depth       = 1; //only used for TextureType::TYPE_3D
        u32 layerCount  = 1; //only changed if not using TextureType::TYPE_2D
        u32 mipMapCount = 1; //how many downsampled textures are allowed
    };

    class LIB_API Texture
    {
    friend class Viewport;
    friend class Shader;
    friend class Mesh;
    friend class Material;
    friend class KalaGraphics::PrimitiveWidgets::Text;
    friend class KalaGraphics::CompositeWidgets::Button;
    friend struct default_delete<Texture>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<Texture>& GetRegistry();

        KNODISCARD
		static bool IsVerboseLoggingEnabled();
        static void SetVerboseLoggingState(bool state);

        //Either initialize a blank white 1x1 texture, or pass data via textureData
        KNODISCARD
		static Texture* Initialize(
            u32 shaderID,
            TextureData&& textureData = {});

        KNODISCARD
		u32 GetID() const;

        KNODISCARD
		u32 GetShaderID() const;
        void SetShaderID(u32 newID);

        //Returns all materials and in which slot in each material is this texture held in
        KNODISCARD
		const vector<pair<u32, array<bool, 11>>>& GetMaterialIDs() const;

        KNODISCARD
        u32 GetTextWidgetID() const;

        KNODISCARD
        u32 GetButtonWidgetID() const;

        //converts linear RGB to sRGB color
        vec4 ToSRGB(vec4&& linearColor);

        //Converts sRGB color to linear RGB color
        vec4 ToLinear(vec4&& sRGBColor);

        //Fill this texture with fixed color and transparency
        void FillColor(vec4&& newValue);

        KNODISCARD
		const vector<u8>& GetPixelData() const;
        void SetPixelData(vector<u8>&& newPixelData);

        KNODISCARD
		TexturePixelFormat GetPixelFormat() const;
        void SetPixelFormat(TexturePixelFormat newFormat);

        KNODISCARD
		TextureType GetType() const;
        void SetType(TextureType newType);

        KNODISCARD
		TextureFilterMode GetFilterMode() const;
        void SetFilterMode(TextureFilterMode newFilter);

        KNODISCARD
		TextureShadowMapMode GetShadowMapMode() const;
        void SetShadowMapMode(TextureShadowMapMode newMode);

        KNODISCARD
		TextureWrapMode GetWrapMode() const;
        void SetWrapMode(TextureWrapMode newWrap);

        KNODISCARD
		TextureBorderColor GetBorderColor() const;
        void SetBorderColor(TextureBorderColor newColor);

        KNODISCARD
		bool IsAnisotropyEnabled() const;
        void SetAnisotropyState(bool newValue);

        KNODISCARD
		vec2 GetSize() const;
        void SetSize(vec2 newSize);

        KNODISCARD
		u32 GetDepth() const;
        void SetDepth(u32 newDepth);

        KNODISCARD
		u32 GetLayerCount() const;
        void SetLayerCount(u32 newCount);

        KNODISCARD
		u8 GetMipMapCount() const;
        void SetMipMapCount(u8 newCount);

        void Destroy();
    private:
        ~Texture();

        void UpdateTextureData();

        void UploadPixelData(VkCommandBuffer vkCommandBuffer);
        void GenerateMipMaps(VkCommandBuffer vkCommandBuffer);

        void ClearAllMaterialTextures();

        u32 ID{};
        u32 shaderID{};

        //materials that contain this texture
        vector<pair<u32, array<bool, 11>>> materialIDs{};

        u32 textWidgetID{};
        u32 buttonWidgetID{};

        bool isRootTexture{};

        //set to true if any texture-breaking data was adjusted
        bool isDirty = true;

        vector<u8> pixelData{};

        TexturePixelFormat format{};
        TextureType type{};
        TextureFilterMode filterMode{};
        TextureShadowMapMode shadowMode{};
        TextureWrapMode wrapMode{};
        TextureBorderColor borderColor{};

        bool useAnisotropy{};

        vec2 size{};
        u32 depth{};
        u32 layerCount{};
        u8 mipMapCount{};

        VkBuffer vkTexBuffer{};
        u64 pixelDataSize{};
        VmaAllocation vmaTexAllocation{};
        void* texMappedPtr{};

        VkSampler vkSampler{};
        VkImage vkImage{};
        VkImageView vkImageView{};
        VmaAllocation vmaImageAllocation{};

        VkDescriptorSet vkDescriptorSet{};
    };
}