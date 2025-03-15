//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <iostream>

//external
#include "glm.hpp"

//engine
#include "component.hpp"

namespace Graphics::Components
{
    using glm::vec3;
    using std::cout;

    class TransformComponent : public Component
    {
    public:
        TransformComponent(
            const vec3& position = vec3(0.0f),
            const vec3& rotation = vec3(0.0f),
            const vec3& scale = vec3(1.0f)) : 
            position(position), 
            rotation(rotation), 
            scale(scale) {}

        void SetPosition(const vec3& newPosition) { position = newPosition; }
        void SetRotation(const vec3& newRotation) { rotation = newRotation; }
        void SetScale(const vec3& newScale) { scale = newScale; }

        const vec3& GetPosition() const { return position; }
        const vec3& GetRotation() const { return rotation; }
        const vec3& GetScale() const { return scale; }

        type_index GetType() const override { return typeid(TransformComponent); }
    private:
        vec3 position;
        vec3 rotation;
        vec3 scale;
    };
}