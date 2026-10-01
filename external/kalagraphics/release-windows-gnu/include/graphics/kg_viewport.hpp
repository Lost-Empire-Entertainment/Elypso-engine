//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <functional>
#include <filesystem>

#include "core_utils.hpp"
#include "math_utils.hpp"
#include "key_standards.hpp"

#include "core/kg_registry.hpp"

namespace KalaGraphics::Graphics
{
    using KalaHeaders::KalaMath::vec2;
    using KalaHeaders::KalaMath::vec4;

    using KalaHeaders::KalaKeyStandards::KeyboardButton;
    using KalaHeaders::KalaKeyStandards::MouseButton;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::string;
    using std::string_view;
    using std::vector;
    using std::unordered_map;
    using std::array;
    using std::pair;
    using std::function;
    using std::filesystem::path;
    using std::default_delete;

    static constexpr u32 MIN_VIEWPORT_SIZE = 50;

    enum class ViewportAnchorPosition : u8
    {
        P_DEFAULT = 0,

        P_BOTTOM_LEFT = 1,
        P_BOTTOM_RIGHT = 2,

        P_TOP_LEFT = 3,
        P_TOP_RIGHT = 4,
        
        P_CENTER = 5
    };

    enum class ViewportStaticSize : u8
    {
        //4:3

        VP_640_480 = 0,
        VP_800_600 = 1,
        VP_1024_768 = 2,
        VP_1600_1200 = 3,

        //16:9

        VP_1280_720 = 4,
        VP_1600_900 = 5,
        VP_1920_1080 = 6,
        VP_2560_1440 = 7,
        VP_3840_2160 = 8,

        //16:10

        VP_1280_800 = 9,
        VP_1680_1050 = 10,
        VP_1920_1200 = 11,
        VP_2560_1600 = 12,

        //21:9

        VP_2560_1080 = 13,
        VP_3440_1440 = 14,
        VP_5120_2160 = 15,

        //32:9

        VP_3840_1080 = 16,
        VP_5120_1440 = 17
    };

    enum class ViewportType : u8
    {
        //Standard fully filled viewport, no letterboxing, no fixed resolution
        VP_FILL = 0,

        //Fixed resolution, uses one of the chosen static resolutions
        //and tries to fit it while preserving as much of the viewport as possible,
        //leaves letterboxing to where aspect ratio doesn't fit viewport
        VP_FIT = 1,

        //Same as fit, except it clamps to static viewport size,
        //so if viewport is bigger than its static size then black bars appear on all edges
        VP_CENTER = 2,

        //User-defined scissor area, the only mode where manual scissor size and offset are allowed,
        //draws black boxes to area outside of scissor range
        VP_CUSTOM = 3
    };

    enum class RootShaderTarget : u8
    {
        T_UNLIT = 0,  //unlit_vert.spv + unlit_frag.spv
        T_RECT  = 1,  //ui_rect_vert.spv + ui_rect_frag.spv
        T_FONT  = 2   //ui_font_vert.spv + ui_font_frag.spv
    };
    
    enum class RootShaderType : u8
    {
        S_VERT_UNLIT = 0, //unlit_vert.spv
        S_VERT_RECT  = 1, //ui_rect_vert.spv
        S_VERT_FONT  = 2, //ui_font_vert.spv

        S_FRAG_UNLIT = 3, //unlit_frag.spv
        S_FRAG_RECT  = 4, //ui_rect_frag.spv
        S_FRAG_FONT  = 5  //ui_font_frag.spv
    };

    struct RootShaderTypeData
    {
        RootShaderType shaderType{};
        path shaderPath{};
    };

    struct RootShader
    {
        RootShaderTarget shaderTarget{};
        u32 shaderID{};

        RootShaderTypeData vertShader{};
        RootShaderTypeData fragShader{};
    };

    //TODO: check root shader validity with spir-v reflect

    class LIB_API Viewport
    {
    friend class GraphicsContext;
    friend class Shader;
    friend class HitTest;
    friend class Material;
    friend class Camera;
    friend class Texture;
    friend class Mesh;
    friend struct default_delete<Viewport>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<Viewport>& GetRegistry();

        KNODISCARD
		static string GetStaticName(ViewportStaticSize vpSize);
        KNODISCARD
		static vec2 GetStaticValue(ViewportStaticSize vpSize);

        //Create a blank viewport with optional viewport type toggle,
        //defaults to dynamic viewport with fill type
        KNODISCARD
		static Viewport* Initialize(
            u32 contextID,
            ViewportType type = {});

        KNODISCARD
		u32 GetID() const;
        KNODISCARD
		u32 GetContextID() const;

        KNODISCARD
        u32 GetTargetViewportID() const;

        KNODISCARD
		u32 GetPrimary3DCameraID() const;
        KNODISCARD
		u32 GetPrimary2DCameraID() const;

        KNODISCARD
		const vector<u32>& GetShader3DIDs() const;
        KNODISCARD
		const vector<u32>& GetShader2DIDs() const;

        //The ID of this root shader
        u32 GetRootShaderID(RootShaderTarget rootShaderTarget);

        //Returns true if this viewport is
        //currently being detected by the Hit Test logic
        KNODISCARD
        bool IsHovered() const;

        KNODISCARD
        bool IsVisible() const;
        void SetVisibleState(bool newValue);

        //Returns true if this viewport is the primary viewport
        //for its graphics context, it cannot be destroyed
        KNODISCARD
		bool IsRootViewport() const;

        //If true then this viewport scales dynamically
        //relative to the size of the graphics context size,
        //viewport size cannot be changed manually if this is true
        KNODISCARD
        bool IsDynamicResizeEnabled() const;
        void SetDynamicResizeState(bool newValue);

        KNODISCARD
        ViewportType GetType() const;
        void SetType(ViewportType newType);

        KNODISCARD
        u8 GetDrawOrderIndex() const;
        //Set the viewport draw order, set sortNow to true
        //if you want this call to sort all viewports, 
        //otherwise the next global update will sort all viewports
        void SetDrawOrderIndex(
            u8 newValue,
            bool sortNow = false);

        KNODISCARD
        const vec4& GetBackgroundColor() const;
        //Assign a new background color, clamped between 0 and 1
        void SetBackgroundColor(vec4&& newColor);

        KNODISCARD
        const vec4& GetLetterboxColor() const;
        //Assign a new color to the scissor area, clamped between 0 and 1
        void SetLetterboxColor(vec4&& newColor);

        //Returns the true pixel coordinates of the requested position
        //relative to letterboxing, useful for accurate 2D UI position calculations
        KNODISCARD
        vec2 GetAnchorPosition(ViewportAnchorPosition pos) const;

        //Returns current viewport size whether its been set as static or dynamic
        KNODISCARD
		vec2 GetSize() const;
        //Set static viewport size, only adjusts aspect ratio, scissor size and scissor offset
        void SetSize(ViewportStaticSize newValue);
        //Set dynamic viewport size
        void SetSize(vec2 newValue);

        KNODISCARD
		vec2 GetOffset() const;
        void SetOffset(vec2 newValue);

        KNODISCARD
		vec2 GetScissorSize() const;
        void SetScissorSize(vec2 newValue);

        KNODISCARD
		vec2 GetScissorOffset() const;
        void SetScissorOffset(vec2 newValue);

        //Called while hovered 
        void SetHoverCallback(function<void()>&& newValue);
        //Called once when hover starts
        void SetOnHoverStartCallback(function<void()>&& newValue);
        //Called once when hover exits
        void SetOnHoverExitCallback(function<void()>&& newValue);

        void SetKeyHeldCallback(
            KeyboardButton btn, 
            function<void()>&& newValue);
        void SetKeyPressedCallback(
            KeyboardButton btn, 
            function<void()>&& newValue);
        void SetKeyReleasedCallback(
            KeyboardButton btn, 
            function<void()>&& newValue); 

        void SetMouseButtonHeldCallback(
            MouseButton btn, 
            function<void()>&& newValue);
        void SetMouseButtonPressedCallback(
            MouseButton btn, 
            function<void()>&& newValue);
        void SetMouseButtonReleasedCallback(
            MouseButton btn, 
            function<void()>&& newValue);
        void SetMouseButtonDoubleClickedCallback(
            MouseButton btn, 
            function<void()>&& newValue);
        void SetMouseButtonDraggingCallback(
            MouseButton btn, 
            function<void(vec2)>&& newValue);

        void SetScrollUpCallback(function<void(f32)>&& newValue);
        void SetScrollDownCallback(function<void(f32)>&& newValue);

        void Destroy();
    private:
        ~Viewport();

        vector<const RootShader*> GetAllRootShaders();

        void Sort3DMeshes();
        void Sort2DMeshes();

        void Update(u32 imageIndex);

        void UpdateViewportSize();

        u32 ID{};
        u32 contextID{};
        u32 hitTestID{};

        u32 primary3DCameraID{};
        u32 primary2DCameraID{};

        //the last shader that was bound during runtime loop,
        //used for meshes to prevent binding the same shader and camera again

        u32 lastBoundShader3DID{};
        u32 lastBoundShader2DID{};

        vector<u32> shader3DIDs{};
        vector<u32> shader2DIDs{};

        vector<u32> sorted3DOpaqueMeshes{};
        vector<u32> sorted3DTransparentMeshes{};

        vector<u32> sorted2DOpaqueMeshes{};
        vector<u32> sorted2DTransparentMeshes{};

        array<RootShader, 3> rootShaderTable{};

        //used only to prevent viewport from removing its ID from
        //graphics context viewport IDs list if the graphics context
        //destroy function called the destroy function of this viewport 
        bool isDestroyingGraphicsContext{};

        bool isVisible = true;

        bool isRootViewport{};

        bool isDynamicResizeEnabled = true;

        bool is3DMeshSortDirty{};
        bool is2DMeshSortDirty{};

        ViewportType viewportType{};

        vec2 posTopLeft{};
        vec2 posTopRight{};
        vec2 posBottomLeft{};
        vec2 posBottomRight{};
        vec2 posCenter{};

        //always starts at 1, can never be 0 except for root viewport
        u8 drawOrderIndex = 1;

        vec4 viewportBackgroundColor = vec4{ 0.0f, 1.0f, 0.0f, 1.0f };
        vec4 viewportLetterboxColor = vec4{ 0.0f, 0.0f, 0.0f, 1.0f };

        ViewportStaticSize viewportStaticSize = ViewportStaticSize::VP_1920_1080;
        vec2 viewportDynamicSize = 100;
        
        //pushes the drawable area down and right if x and y are positive
        vec2 viewportOffset{};

        //cuts everything outside of this area,
        //gpu can only draw clear color there
        vec2 scissorSize{};
        //pushes the clipped area down and right if x and y are positive
        vec2 scissorOffset{};

        //callbacks

        function<void()> hoverCallback{};
        function<void()> onHoverStartCallback{};
        function<void()> onHoverExitCallback{};

        unordered_map<KeyboardButton, function<void()>> keyHeldCallbacks{};
        unordered_map<KeyboardButton, function<void()>> keyPressedCallbacks{};
        unordered_map<KeyboardButton, function<void()>> keyReleasedCallbacks{};

        unordered_map<MouseButton, function<void()>> mouseButtonHeldCallbacks{};
        unordered_map<MouseButton, function<void()>> mouseButtonPressedCallbacks{};
        unordered_map<MouseButton, function<void()>> mouseButtonReleasedCallbacks{};
        unordered_map<MouseButton, function<void()>> mouseButtonDoubleClickedCallbacks{};
        unordered_map<MouseButton, function<void(vec2)>> mouseButtonDraggingCallbacks{};

        function<void(f32)> scrollUpCallback{};
        function<void(f32)> scrollDownCallback{};
    };
}