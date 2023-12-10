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

//external
#include "glad.h"

//engine
#include "input.h"
#include "render.h"
#include "console.h"
#include "timeManager.h"

#include <iostream>
#include <string>

using namespace std;
using namespace glm;
using namespace Graphics;
using namespace Core;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
    void Input::InputSetup()
    {
        key[Key::Escape] = GLFW_KEY_ESCAPE;
        key[Key::W] = GLFW_KEY_W;
        key[Key::S] = GLFW_KEY_S;
        key[Key::A] = GLFW_KEY_A;
        key[Key::D] = GLFW_KEY_D;
        key[Key::Space] = GLFW_KEY_SPACE;
        key[Key::Left_control] = GLFW_KEY_LEFT_CONTROL;
        key[Key::Left_shift] = GLFW_KEY_LEFT_SHIFT;
        key[Key::Z] = GLFW_KEY_Z;
        key[Key::X] = GLFW_KEY_X;
    }

    Input::Input(GLFWwindow* window, float sensitivity) : 
        window(window), 
        yaw(-90.0f), 
        pitch(0.0f), 
        lastX(0.0), lastY(0.0), 
        firstMouse(true), 
        sensitivity(sensitivity),
        cameraPos(vec3(0.0f, 0.0f, 3.0f)), 
        cameraFront(vec3(0.0f, 0.0f, -1.0f)), 
        cameraUp(vec3(0.0f, 1.0f, 0.0f)) {}

	void Input::ProcessInput(GLFWwindow* window)
	{
        Input::ProcessKeyboardInput(Render::window);

		//process mouse movement
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
			{
				Render::camera.ProcessMouseMovement(xpos, ypos);
			});
		Render::camera.ProcessMouseMovement(mouseX, mouseY);
		Render::cameraSpeed = static_cast<float>(2.5f * TimeManager::deltaTime);
	}

    void Input::ProcessKeyboardInput(GLFWwindow* window)
    {
        bool isLeftShiftPressed = glfwGetKey(window, static_cast<int>(key[Key::Left_shift])) == GLFW_PRESS;
        float currentSpeed = Render::cameraSpeed;
        if (isLeftShiftPressed) currentSpeed = 2.0f;
        else                    currentSpeed = 1.0f;

        //toggle fullscreen
        if (glfwGetKey(window, static_cast<int>(key[Key::Z])) == GLFW_PRESS)
        {
            Render::ToggleFullscreenMode(Render::window, Render::enableFullscreen);
        }
        //toggle vsync
        if (glfwGetKey(window, static_cast<int>(key[Key::X])) == GLFW_PRESS)
        {
            Render::useMonitorRefreshRate = !Render::useMonitorRefreshRate;
        }

        //escape
        if (glfwGetKey(window, static_cast<int>(key[Key::Escape])) == GLFW_PRESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::SUCCESS,
                "User pressed ESC key to shut down engine...\n\n");

            glfwSetWindowShouldClose(window, true);
        }

        vec3 front = Render::camera.GetFront();
        vec3 right = Render::camera.GetRight();

        //camera forwards
        if (glfwGetKey(window, static_cast<int>(key[Key::W])) == GLFW_PRESS)
        {
            Render::cameraPos += Render::cameraSpeed * currentSpeed * front;
        }
        //camera backwards
        if (glfwGetKey(window, static_cast<int>(key[Key::S])) == GLFW_PRESS)
        {
            Render::cameraPos -= Render::cameraSpeed * currentSpeed * front;
        }
        //camera left
        if (glfwGetKey(window, static_cast<int>(key[Key::A])) == GLFW_PRESS)
        {
            Render::cameraPos -= Render::cameraSpeed * currentSpeed * right;
        }
        //camera right
        if (glfwGetKey(window, static_cast<int>(key[Key::D])) == GLFW_PRESS)
        {
            Render::cameraPos += Render::cameraSpeed * currentSpeed * right;
        }
        //camera up
        if (glfwGetKey(window, static_cast<int>(key[Key::Space])) == GLFW_PRESS)
        {
            //local Y
            Render::cameraPos += Render::cameraUp * Render::cameraSpeed * currentSpeed;
        }
        //camera down
        if (glfwGetKey(window, static_cast<int>(key[Key::Left_control])) == GLFW_PRESS)
        {
            //local Y
            Render::cameraPos -= Render::cameraUp * Render::cameraSpeed * currentSpeed;
        }
    }

    void Input::ProcessMouseMovement(double xpos, double ypos) {
        if (firstMouse) 
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        double xOffset = xpos - lastX;
        double yOffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if (yaw > 359.99f || yaw < -359.99f)
        {
            yaw = 0.0f;
        }

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        vec3 front{};
        front.x = cos(radians(yaw)) * cos(radians(pitch));
        front.y = sin(radians(pitch));
        front.z = sin(radians(yaw)) * cos(radians(pitch));
        cameraFront = normalize(front);
    }

    void Input::ProcessScrollWheel(GLFWwindow* window, double xoffset, double yoffset) 
    {
        Render::fov += yoffset * 3;

        if (Render::fov > 110) Render::fov = 110;
        if (Render::fov < 50) Render::fov = 50;
    }

    mat4 Input::GetViewMatrix() const
    {
        return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
}