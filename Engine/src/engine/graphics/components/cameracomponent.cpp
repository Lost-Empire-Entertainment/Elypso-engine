//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

//external
#include "gtc/quaternion.hpp"

//engine
#include "cameracomponent.hpp"
#include "gameobject.hpp"

using glm::lookAt;
using glm::quat;

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
        aspectRatio(aspectRatio),
        cameraFront(cameraFront),
        cameraUp(cameraUp),
        lastRotation(lastRotation) {}

    void CameraComponent::RotateCamera(double deltaX, double deltaY)
    {
        if (isEnabled)
        {
            deltaX *= sensitivity;
            deltaY *= sensitivity;

            yaw += static_cast<float>(deltaX);
            pitch += static_cast<float>(deltaY);

            //clamp yaw and pitch to prevent unnatural rotation
            if (yaw > 359.99f
                || yaw < -359.99f)
            {
                yaw = 0.0f;
            }
            if (pitch > 89.99f) pitch = 89.99f;
            if (pitch < -89.99f) pitch = -89.99f;

            //update camera front vector based on new yaw and pitch
            vec3 front{};
            front.x = cos(radians(yaw)) * cos(radians(pitch));
            front.y = sin(radians(pitch));
            front.z = sin(radians(yaw)) * cos(radians(pitch));
            cameraFront = normalize(front);

            //
            // THESE AFFECT THE CAMERA TRANSFORM ROTATION,
            // THEY DONT AFFECT THE CAMERA ROTATION ITSELF
            //

            const vec3& transformRot = GetRotation();
            auto tc = GetOwner()->GetComponent<TransformComponent>();

            //eulerDeg.x = pitch (deg)
            //eulerDeg.y = yaw (deg)
            //eulerDeg.z = roll (deg)
            tc->SetRotation(transformRot);
        }
    }

    void CameraComponent::SetRotation(const vec3& transformRotation)
    {
        const shared_ptr<GameObject>& thisCamera = this->GetOwner();
        auto tc = thisCamera->GetComponent<TransformComponent>();

        yaw = -transformRotation.y;             //yaw is stored in the Y component
        pitch = -(transformRotation.x + 90.0f); //pitch is stored in X, adjusted by 90 degrees

        //clamp yaw and pitch to prevent unnatural rotation
        if (yaw > 359.99f
            || yaw < -359.99f)
        {
            yaw = 0.0f;
        }
        if (pitch > 89.99f) pitch = 89.99f;
        if (pitch < -89.99f) pitch = -89.99f;

        //update camera front vector based on new yaw and pitch
        vec3 front{};
        front.x = cos(radians(yaw)) * cos(radians(pitch));
        front.y = sin(radians(pitch));
        front.z = sin(radians(yaw)) * cos(radians(pitch));
        cameraFront = normalize(front);
    }
    const vec3 CameraComponent::GetRotation() const
    {
        //rotate around local Y-axis (left/right)
        quat qYaw = angleAxis(radians(-yaw), vec3(0, 1, 0));
        //rotate around local X-axis (up/down)
        quat qPitch = angleAxis(radians(-pitch), vec3(1, 0, 0));

        //rotate the camera's forward direction 90 degrees to the left
        quat qOffset = angleAxis(radians(90.0f), vec3(0, 1, 0));

        quat correctedCamera = qOffset * qYaw * qPitch;

        vec3 euler = degrees(eulerAngles(correctedCamera));

        //apply rotation to transform component
        auto tc = GetOwner()->GetComponent<TransformComponent>();
        vec3 eulerDeg = degrees(eulerAngles(correctedCamera));
        eulerDeg.x -= 90.0f; //correct pitch to look up instead of down

        return eulerDeg;
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