//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

//engine
#include "cameracomponent.hpp"
#include "gameobject.hpp"

using glm::lookAt;

using Graphics::Shape::GameObject;

namespace Graphics::Components
{
    CameraComponent::CameraComponent(
        bool isEnabled,
        float speed,
        float sensitivity,
        float yaw,
        float pitch,
        float lastX,
        float lastY,
        float aspectRatio,
        vec3 cameraFront,
        vec3 cameraUp,
        vec3 lastRotation) :
        isEnabled(isEnabled),
        speed(speed),
        sensitivity(sensitivity),
        yaw(yaw),
        pitch(pitch),
        lastX(lastX), 
        lastY(lastY),
        cameraFront(cameraFront),
        cameraUp(cameraUp),
        lastRotation(lastRotation) {}

    void CameraComponent::RotateCamera(double deltaX, double deltaY)
    {
        const shared_ptr<GameObject>& thisCamera = this->GetOwner();
        auto cc = thisCamera->GetComponent<CameraComponent>();

        if (cc->isEnabled)
        {
            deltaX *= cc->sensitivity;
            deltaY *= cc->sensitivity;

            cc->yaw += static_cast<float>(deltaX);
            cc->pitch += static_cast<float>(deltaY);

            //clamp yaw and pitch to prevent unnatural rotation
            if (cc->yaw > 359.99f
                || cc->yaw < -359.99f)
            {
                cc->yaw = 0.0f;
            }
            if (cc->pitch > 89.99f) cc->pitch = 89.99f;
            if (cc->pitch < -89.99f) cc->pitch = -89.99f;

            //update camera front vector based on new yaw and pitch
            vec3 front{};
            front.x = cos(radians(cc->yaw)) * cos(radians(cc->pitch));
            front.y = sin(radians(cc->pitch));
            front.z = sin(radians(cc->yaw)) * cos(radians(cc->pitch));
            cc->cameraFront = normalize(front);
        }
    }

    mat4 CameraComponent::GetViewMatrix() const
    {
        const shared_ptr<GameObject>& thisCamera = this->GetOwner();
        auto cc = thisCamera->GetComponent<CameraComponent>();
        auto transform = thisCamera->GetComponent<TransformComponent>();

        return lookAt(
            transform->GetPosition(),
            transform->GetPosition() + cc->GetFront(),
            cameraUp);
    }
}