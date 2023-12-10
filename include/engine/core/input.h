//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

#pragma once

//external
#include "glm.hpp"
#include "matrix_transform.hpp" //glm/gtc
#include "glfw3.h"

#include <unordered_map>

using namespace std;
using namespace glm;

namespace Core
{
    class Input
    {
    public:
        enum class Key : char
        {
            Escape, //quit
            W, //camera forwards
            S, //camera backwards
            A, //camera left
            D, //camera right
            Space, //camera up
            Left_control, //camera down
            Left_shift, //camera faster movement
            Z, //toggle fullscreen mode
            X //toggle vsync
        };

        static inline unordered_map<Key, int> key;

        Input(GLFWwindow* window, float sensitivity = 0.05f);
        mat4 GetViewMatrix() const;

        static void InputSetup();
        static void ProcessInput(GLFWwindow* window);
        vec3 GetFront() const { return cameraFront; }
        vec3 GetRight() const { return normalize(cross(cameraFront, cameraUp)); }

        vec3 GetCameraRotation() const { return vec3(yaw, pitch, 0); }
        static void ProcessScrollWheel(GLFWwindow* window, double xoffset, double yoffset);
    private:
        GLFWwindow* window;
        float yaw;
        float pitch;
        double lastX;
        double lastY;
        bool firstMouse;
        float sensitivity;

        vec3 cameraPos;
        vec3 cameraFront;
        vec3 cameraUp;

        static inline bool wasFullscreenKeyPressed;
        static inline bool wasVSYNCKeyPressed;

        void ProcessMouseMovement(double xpos, double ypos);
        static void ProcessKeyboardInput(GLFWwindow* window);
    };
}