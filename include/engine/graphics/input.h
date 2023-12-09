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

#ifndef GRAPHICS_INPUT_H
#define GRAPHICS_INPUT_H

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
            Escape,
            Enter,
            W,
            S,
            A,
            D,
            Space,
            Left_control
        };

        static inline unordered_map<Key, int> key;

        Input(GLFWwindow* window, float sensitivity = 0.05f);
        mat4 GetViewMatrix() const;

        static inline float fov;

        static void InputSetup();
        static void ProcessInput(GLFWwindow* window);

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

        void ProcessMouseMovement(double xpos, double ypos);
        static void ProcessKeyboardInput(GLFWwindow* window);
    };
}

#endif