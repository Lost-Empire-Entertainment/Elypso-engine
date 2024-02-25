//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#pragma once

#include <unordered_map>
#include <string>

//external
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //two matrix_transform.hpp files exist in glm
#include "glfw3.h"

//engine
#include "input.hpp"

using glm::vec3;
using glm::mat4;
using glm::radians;
using std::unordered_map;
using std::string;

namespace Core
{
    class Input
    {
    public:
        static inline float cameraSpeed = 2.5f;

        static inline vec3 lastKnownRotation = vec3(0);
        static inline float moveSpeedMultiplier = 1.0f;
        static inline float nearClip = 0.01f;
        static inline float farClip = 100.0f;
        static inline float fov = 90.0f;
        static inline float aspectRatio = 0.0f;
        static inline bool cameraEnabled;
        static inline bool cameraModeSwitched;

        static inline bool leftMouseHeld;
        static inline double lastMouseX;
        static inline double lastMouseY;
        static inline float objectSensitivity = 0.1f;
        static inline vec3 newObjectPosition;
        static inline vec3 newObjectRotation;
        static inline vec3 newObjectScale;

        static inline string axis = "X";
        enum class ObjectAction
        {
            none,
            move,
            rotate,
            scale
        };
        static inline ObjectAction objectAction = ObjectAction::none;

        static inline bool printIMGUIToConsole;
        static inline bool printInputToConsole;
        static inline bool printSelectRayDirectionToConsole;

        Input(GLFWwindow* window, float sensitivity = 0.05f);
        mat4 GetViewMatrix() const;

        static void ProcessInput(GLFWwindow* window);

        vec3 GetCameraPosition() const { return cameraPos; }
        void SetCameraPosition(const vec3& newPosition) { cameraPos = newPosition; }
        vec3 GetFront() const { return cameraFront; }
        vec3 GetRight() const { return normalize(cross(cameraFront, cameraUp)); }
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

        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void MouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
    private:
        GLFWwindow* window;
        float yaw;
        float pitch;
        double lastX;
        double lastY;
        bool firstMouse;
        float sensitivity;

        static inline bool printCheck;

        vec3 cameraPos;
        vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
        vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

        void ProcessMouseMovement(double xpos, double ypos);
        static void ProcessKeyboardInput(GLFWwindow* window);
    };
}