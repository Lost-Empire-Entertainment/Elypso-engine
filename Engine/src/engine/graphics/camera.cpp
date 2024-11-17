//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "matrix_transform.hpp"

//engine
#include "camera.hpp"
#include "render.hpp"

namespace Graphics
{
    Camera::Camera(GLFWwindow* window, float sensitivity)
        : window(window),
        yaw(-90.0f),
        pitch(0.0f),
        lastX(0.0f), lastY(0.0f),
        sensitivity(sensitivity),
        cameraPos(glm::vec3(0.0f, 1.0f, 0.0f)),
        cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
        cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)) {}

    void Camera::RotateCamera(double deltaX, double deltaY)
    {
        if (Render::camera.cameraEnabled)
        {
            deltaX *= Render::camera.sensitivity;
            deltaY *= Render::camera.sensitivity;

            Render::camera.yaw += static_cast<float>(deltaX);
            Render::camera.pitch += static_cast<float>(deltaY);

            //clamp yaw and pitch to prevent unnatural rotation
            if (Render::camera.yaw > 359.99f
                || Render::camera.yaw < -359.99f)
            {
                Render::camera.yaw = 0.0f;
            }
            if (Render::camera.pitch > 89.99f) Render::camera.pitch = 89.99f;
            if (Render::camera.pitch < -89.99f) Render::camera.pitch = -89.99f;

            //update camera front vector based on new yaw and pitch
            vec3 front{};
            front.x = cos(radians(Render::camera.yaw)) * cos(radians(Render::camera.pitch));
            front.y = sin(radians(Render::camera.pitch));
            front.z = sin(radians(Render::camera.yaw)) * cos(radians(Render::camera.pitch));
            Render::camera.cameraFront = normalize(front);
        }
    }

    mat4 Camera::GetViewMatrix() const
    {
        return lookAt(
            Render::camera.GetCameraPosition(),
            Render::camera.GetCameraPosition() +
            Render::camera.GetFront(),
            cameraUp);
    }
}