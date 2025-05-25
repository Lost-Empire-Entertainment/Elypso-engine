//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <typeindex>

//external
#include "glm.hpp"

//engine
#include "component.hpp"

namespace Graphics::Components
{
    using std::type_index;
    using glm::vec3;

    class LightComponent : public Component
    {
    public:
        enum class LightType
        {
            Point,
            Spot,
            Directional
        };

        LightComponent(
            LightType type,
            const vec3& diffuse = vec3(1.0f),
            float intensity = 1.0f, 
            float farPlane = 1.0f,
            float nearPlane = 0.5f,
            float innerAngle = 12.5f, 
            float outerAngle = 37.5f) : 
            lightType(type),
            diffuse(diffuse), 
            intensity(intensity), 
            farPlane(farPlane),
            nearPlane(nearPlane),
            innerAngle(innerAngle), 
            outerAngle(outerAngle) {}

        void SetDiffuse(const vec3& newDiffuse) { diffuse = newDiffuse; }
        void SetIntensity(float newIntensity) { intensity = newIntensity; }
        void SetFarPlane(float newFarPlane) { farPlane = newFarPlane; }
        void SetNearPlane(float newNearPlane) { nearPlane = newNearPlane; }
        void SetInnerAngle(float newInnerAngle) { innerAngle = newInnerAngle; }
        void SetOuterAngle(float newOuterAngle) { outerAngle = newOuterAngle; }

        const vec3& GetDiffuse() const { return diffuse; }
        float GetIntensity() const { return intensity; }
        float GetFarPlane() const { return farPlane; }
        float GetNearPlane() const { return nearPlane; }
        float GetInnerAngle() const { return innerAngle; }
        float GetOuterAngle() const { return outerAngle; }
        LightType GetLightType() const { return lightType; }

        type_index GetType() const override { return typeid(LightComponent); }
    private:
        LightType lightType;

        //common properties
        vec3 diffuse;
        float intensity;

        //point and spotlight properties
        float farPlane;
        float nearPlane;

        //spotlight properties
        float innerAngle;
        float outerAngle;
    };
}