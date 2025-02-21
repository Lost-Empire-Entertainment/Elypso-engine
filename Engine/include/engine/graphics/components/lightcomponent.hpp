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

        //spotlight
        LightComponent(
            LightType type,
            const vec3& diffuse = vec3(1.0f),
            float intensity = 1.0f, 
            float distance = 1.0f, 
            float innerAngle = 12.5f, 
            float outerAngle = 37.5f) : 
            lightType(type),
            diffuse(diffuse), 
            intensity(intensity), 
            distance(distance), 
            innerAngle(innerAngle), 
            outerAngle(outerAngle) {}

        void SetDiffuse(const vec3& newDiffuse) { diffuse = newDiffuse; }
        void SetIntensity(float newIntensity) { intensity = newIntensity; }
        void SetDistance(float newDistance) { distance = newDistance; }
        void SetInnerAngle(float newInnerAngle) { innerAngle = newInnerAngle; }
        void SetOuterAngle(float newOuterAngle) { outerAngle = newOuterAngle; }

        const vec3& GetDiffuse() const { return diffuse; }
        float GetIntensity() const { return intensity; }
        float GetDistance() const { return distance; }
        float GetInnerAngle() const { return innerAngle; }
        float GetOuterAngle() const { return outerAngle; }
        LightType GetLightType() const { return lightType; }

        type_index GetType() const override { return typeid(LightComponent); }

        void Update(float deltaTime) override {}

    private:
        LightType lightType;

        //common properties
        vec3 diffuse;
        float intensity;

        //point and spotlight properties
        float distance;

        //spotlight properties
        float innerAngle;
        float outerAngle;
    };
}