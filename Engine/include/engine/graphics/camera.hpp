//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "../../../../_external_shared/GLM/glm.hpp"
#include "GLFW/glfw3.h"

namespace Graphics
{
    using glm::mat4;
    using glm::vec3;
    using glm::radians;
    using std::to_string;

    class Camera
    {
    public:
        Camera(GLFWwindow* window, float sensitivity);

        static inline vec3 lastKnownRotation = vec3(0);
        static inline float cameraSpeed = 2.5f;
        static inline float aspectRatio{};
        static inline bool cameraEnabled = false;

        mat4 GetViewMatrix() const;

        static void RotateCamera(double xpos, double ypos);

        vec3 GetCameraPosition() const { return cameraPos; }
        void SetCameraPosition(const vec3& newPosition) { cameraPos = newPosition; }
        vec3 GetFront() const { return cameraFront; }
        vec3 GetRight() const { return normalize(cross(cameraFront, cameraUp)); }
        vec3 GetUp() const { return cameraUp; }
        vec3 GetCameraRotation() const { return vec3(yaw, pitch, 0); }
        void SetCameraRotation(const vec3& newRotation)
        {
            //update internal state (yaw and pitch)
            yaw = newRotation.x;
            pitch = newRotation.y;

            //recalculate cameraFront based on the new yaw and pitch
            vec3 front{};
            front.x = cos(radians(yaw)) * cos(radians(pitch));
            front.y = sin(radians(pitch));
            front.z = sin(radians(yaw)) * cos(radians(pitch));
            cameraFront = normalize(front);
        }
    private:
        GLFWwindow* window{};
        float yaw{};
        float pitch{};
        double lastX{};
        double lastY{};
        float sensitivity{};

        vec3 cameraPos;
        vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
        vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
    };
}