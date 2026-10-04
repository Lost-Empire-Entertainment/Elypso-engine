//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kg_registry.hpp"

struct VkBuffer_T;
using VkBuffer = VkBuffer_T*;

struct VmaAllocation_T;
using VmaAllocation = VmaAllocation_T*;

struct VkDescriptorSet_T;
using VkDescriptorSet = VkDescriptorSet_T*;

namespace KalaGraphics::Graphics
{
    using KalaHeaders::KalaMath::Transform3D;
    using KalaHeaders::KalaMath::vec2;
    using KalaHeaders::KalaMath::vec3;
    using KalaHeaders::KalaMath::mat4;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::default_delete;

    //min and max allowed raw deltatime for mouse
    static constexpr f32 MOUSE_MAX = 100.0f;

    static constexpr f32 SENS_MIN = 0.1f;
    static constexpr f32 SENS_MAX = 10.0f;

    static constexpr f32 SPEED_MIN = 0.1f;
    static constexpr f32 SPEED_MAX = 10.0f;

    static constexpr f32 FOV_MIN = 30.0f;
    static constexpr f32 FOV_MAX = 180.0f;

    static constexpr f32 DRAW_DISTANCE_MIN = 0.001f;
    static constexpr f32 DRAW_DISTANCE_MAX = 10000.0f;

    enum class CameraType : u8
    {
        //2D camera
        CAM_ORTHOGRAPHIC = 0,
        //fps-stype 3D camera
        CAM_PERSPECTIVE = 1
    };

    class LIB_API Camera
    {
    friend class GraphicsContext;
    friend class Viewport;
    friend class Shader;
    friend class Mesh;
    friend struct default_delete<Camera>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<Camera>& GetRegistry();

        KNODISCARD
		static Camera* Initialize(
            u32 shaderID,
            CameraType type = CameraType::CAM_PERSPECTIVE);

        KNODISCARD
		u32 GetID() const;
        KNODISCARD
		u32 GetViewportID() const;

        KNODISCARD
		u32 GetShaderID() const;
        void SetShaderID(u32 newValue);

        KNODISCARD
		u32 GetMeshID() const;
        void SetMeshID(u32 newValue);

        //Pass mouse and keyboard input to this camera,
        //Keyboard is internally clamped to -1, 1,
        //Mouse is clamped to MOUSE_MAX,
        //Vertical is global up-down movement, optional.
        //Pass deltaTime if you wish to also modify camera by that, optional.
        //Calls UpdateCameraData internally, no need to call it separately.
        void Move(
            vec2 mouse,
            vec2 keyboard,
            f32 vertical = {},
            f32 deltaTime = {});

        KNODISCARD
		Transform3D& GetTransform();

        KNODISCARD
		CameraType GetCameraType() const;

        //Is this camera 2D (orthographic) or 3D (perspective)
        KNODISCARD
		bool Is2D();

        KNODISCARD
		f32 GetSpeedMultiplier() const;
        void SetSpeedMultiplier(f32 newSpeed);

        KNODISCARD
		f32 GetSensitivityMultiplier() const;
        void SetSensitivityMultiplier(f32 newSens);

        KNODISCARD
		f32 GetFOV() const;
        void SetFOV(f32 newFOV);

        KNODISCARD
		vec2 GetDrawDistance() const;
        void SetDrawDistance(vec2 newDraw);

        KNODISCARD
		const mat4& GetMatrix() const;

        void Destroy();
    private:
        ~Camera();

        void ClearAllData();

        void UpdateCameraData();

        u32 ID{};
        u32 viewportID{};
        u32 shaderID{};
        u32 meshID{};

        CameraType type = CameraType::CAM_PERSPECTIVE;

        Transform3D transform{};

        f32 speedMultiplier = 1.0f;
        f32 sensitivityMultiplier = 1.0f;
        f32 fov = 90.0f;
        vec2 drawDistance = { 0.1f, 1000.0f };

        mat4 projectionMatrix{};
        mat4 orthographicMatrix{};

        bool isDirty{};

        VkBuffer vkCameraUBOBuffer{};
        VmaAllocation vmaCameraUBOAllocation{};
        void* cameraUBOMappedPtr{};

        VkDescriptorSet vkDescriptorSet{};
    };
}