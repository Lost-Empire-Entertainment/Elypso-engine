//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

//engine
#include "cameracomponent.hpp"
#include "gameobject.hpp"

using glm::lookAt;
using glm::quat;
using glm::clamp;

using Graphics::Shape::GameObject;

namespace Graphics::Components
{
    CameraComponent::CameraComponent(
        float speed,
        float sensitivity) :
        speed(speed),
        sensitivity(sensitivity) {}

    void CameraComponent::RotateCamera(double deltaX, double deltaY)
    {
        if (isEnabled)
        {
            deltaX *= sensitivity;
            deltaY *= sensitivity;

            auto transform = GetOwner()->GetComponent<TransformComponent>();
            vec3 rotation = transform->GetRotation();

            //apply yaw (left/right)
            rotation.y += static_cast<float>(deltaX);

            //apply pitch (up/down)
            rotation.x = rotation.x + static_cast<float>(deltaY);
            
            //clamp yaw and pitch to prevent unnatural rotation
            if (rotation.y > 359.99f
                || rotation.y < -359.99f)
            {
                rotation.y = 0.0f;
            }
            if (rotation.x > 89.99f) rotation.x = 89.99f;
            if (rotation.x < -89.99f) rotation.x = -89.99f;

            transform->SetRotation(rotation);
        }
    }

    vec3 CameraComponent::GetFront() const
    {
        auto tc = GetOwner()->GetComponent<TransformComponent>();
        vec3 rot = tc->GetRotation();

        vec3 front{};
        front.x = cos(radians(rot.y)) * cos(radians(rot.x));
        front.y = sin(radians(rot.x));
        front.z = sin(radians(rot.y)) * cos(radians(rot.x));

        return normalize(front);
    }

    mat4 CameraComponent::GetViewMatrix() const
    {
        auto transform = GetOwner()->GetComponent<TransformComponent>();
        return lookAt(
            transform->GetPosition(),
            transform->GetPosition() + GetFront(),
            vec3(0, 1, 0));
    }
}