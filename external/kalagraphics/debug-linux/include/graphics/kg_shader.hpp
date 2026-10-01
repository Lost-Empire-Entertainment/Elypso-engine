//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kg_registry.hpp"

struct VkShaderModule_T;
using VkShaderModule = VkShaderModule_T*;

struct VkDescriptorSetLayout_T;
using VkDescriptorSetLayout = VkDescriptorSetLayout_T*;

struct VkPipelineLayout_T;
using VkPipelineLayout = VkPipelineLayout_T*;

struct VkPipeline_T;
using VkPipeline = VkPipeline_T*;

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
    using KalaHeaders::KalaMath::mat4;
    using KalaHeaders::KalaMath::vec4;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::filesystem::path;
    using std::string;
    using std::string_view;
    using std::vector;
    using std::pair;
    using std::default_delete;

    struct ShaderModuleData
    {
        VkShaderModule vkModule_vert{};
        uintptr_t spvModule_vert{};

        VkShaderModule vkModule_frag{};
        uintptr_t spvModule_frag{};

        bool usingGeom{};
        VkShaderModule vkModule_geom{};
        uintptr_t spvModule_geom{};
    };

    class LIB_API Shader
    {
    friend class GraphicsContext;
    friend class HitTest;
    friend class Viewport;
    friend class Material;
    friend class Camera;
    friend class Texture;
    friend class Mesh;
    friend class KalaGraphics::PrimitiveWidgets::Text;
    friend class KalaGraphics::CompositeWidgets::Button;
    friend default_delete<Shader>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<Shader>& GetRegistry();

        //Create a blank shader that isn't attached to any viewports or mesh and has no data,
        //you must give it a viewport and set shader data to use it for meshes
        KNODISCARD
		static Shader* Initialize(
            u32 viewportID,
            bool is2D,
            path&& vertPath,
            path&& fragPath,
            path&& geomPath = {});

        KNODISCARD
		u32 GetID() const;
        KNODISCARD 
		u32 GetViewportID() const;
        KNODISCARD
		const vector<u32>& GetCameraIDs() const;
        KNODISCARD
		const vector<u32>& GetTextureIDs() const;
        KNODISCARD
		const vector<u32>& GetMeshIDs() const;

        KNODISCARD
        const vector<u32>& GetTextWidgetIDs() const;
        
        KNODISCARD
        const vector<u32>& GetButtonWidgetIDs() const;

        //The ID of the fallback pink and black tile texture
        u32 GetFallbackTextureID() const;
        //The ID of the root white texture
        u32 GetRootTextureID() const;

        KNODISCARD
		bool Is2D() const;

        KNODISCARD
		const vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts();

        void Destroy();
    private:
        ~Shader();

        static void DestroyVkShaderModules(vector<VkShaderModule> modules);

        KNODISCARD
		static const vector<path>& GetRequiredRootVertexShaders();
        KNODISCARD
		static const vector<path>& GetRequiredRootFragmentShaders();

        u32 ID{};
        u32 viewportID{};

        vector<u32> textureIDs{};
        vector<u32> cameraIDs{};
        vector<u32> meshIDs{};

        vector<u32> textWidgetIDs{};
        vector<u32> buttonWidgetIDs{};

        u32 fallbackTextureID{};
        u32 rootTextureID{};

        bool overrideRootDeletePermission{};
        bool isRootShader{};

        //used only to prevent shader from removing its ID from
        //viewport shader IDs list if the viewport
        //destroy function called the destroy function of this shader 
        bool isDestroyingViewport{};

        bool is2D{};

        bool hasDrawn3DCamera{};
        bool hasDrawn2DCamera{};

        ShaderModuleData shaderModuleData{};
        vector<VkDescriptorSetLayout> descriptorSetLayouts{};
        VkPipelineLayout pipelineLayout{};
        VkPipeline pipeline{};
    };
}