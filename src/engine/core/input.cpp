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
#include "gui.h"

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
        key[Key::CameraForwards] = GLFW_KEY_W;
        key[Key::CameraBackwards] = GLFW_KEY_S;
        key[Key::CameraLeft] = GLFW_KEY_A;
        key[Key::CameraRight] = GLFW_KEY_D;
        key[Key::CameraUp] = GLFW_KEY_SPACE;
        key[Key::CameraDown] = GLFW_KEY_LEFT_CONTROL;
        key[Key::CameraSprint] = GLFW_KEY_LEFT_SHIFT;
        key[Key::ToggleFullscreen] = GLFW_KEY_Z;
        key[Key::ToggleVSYNC] = GLFW_KEY_X;
        key[Key::PrintFPSDebugToConsole] = GLFW_KEY_F1;
        key[Key::PrintIMGUIDebugToConsole] = GLFW_KEY_F2;
        key[Key::PrintInputDebugToConsole] = GLFW_KEY_F3;
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
		Render::cameraSpeed = static_cast<float>(2.5f * mouseSpeedMultiplier * TimeManager::deltaTime);
	}

    void Input::ProcessKeyboardInput(GLFWwindow* window)
    {
        //toggle fullscreen
        int fullscreenKeyState = glfwGetKey(window, static_cast<int>(key[Key::ToggleFullscreen]));
        if (fullscreenKeyState == GLFW_PRESS 
            && !wasFullscreenKeyPressed)
        {
            Render::ToggleFullscreenMode(Render::window, Render::enableFullscreen);

            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "User set fullscreen state to " + to_string(Render::enableFullscreen) + ".\n");

            wasFullscreenKeyPressed = true;
        }
        else if (fullscreenKeyState == GLFW_RELEASE)
        {
            wasFullscreenKeyPressed = false;
        }
        //toggle vsync
        int vsyncKeyState = glfwGetKey(window, static_cast<int>(key[Key::ToggleVSYNC]));
        if (vsyncKeyState == GLFW_PRESS 
            && !wasVSYNCKeyPressed)
        {
            Render::useMonitorRefreshRate = !Render::useMonitorRefreshRate;
            glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);

            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "User set vsync state to " + to_string(Render::useMonitorRefreshRate) + 
                ". Monitor refresh rate: " + to_string(GUI::GetScreenRefreshRate()) + ".\n");

            wasVSYNCKeyPressed = true;
        }
        else if (vsyncKeyState == GLFW_RELEASE)
        {
            wasVSYNCKeyPressed = false;
        }

        //toggle console framerate debug messages
        int fpsKeyState = glfwGetKey(window, static_cast<int>(key[Key::PrintFPSDebugToConsole]));
        if (fpsKeyState == GLFW_PRESS
            && !wasFPSDebugKeyPressed)
        {
            Input::printFPSToConsole = !Input::printFPSToConsole;
            wasFPSDebugKeyPressed = true;
        }
        else if (fpsKeyState == GLFW_RELEASE)
        {
            wasFPSDebugKeyPressed = false;
        }

        if (mouseFocused)
        {
            bool isLeftShiftPressed = glfwGetKey(window, static_cast<int>(key[Key::CameraSprint])) == GLFW_PRESS;
            float currentSpeed = Render::cameraSpeed;
            if (isLeftShiftPressed) currentSpeed = 2.0f * moveSpeedMultiplier;
            else                    currentSpeed = 1.0f * moveSpeedMultiplier;

            vec3 front = Render::camera.GetFront();
            vec3 right = Render::camera.GetRight();

            //camera forwards
            if (glfwGetKey(window, static_cast<int>(key[Key::CameraForwards])) == GLFW_PRESS)
            {
                Render::cameraPos += Render::cameraSpeed * currentSpeed * front;
            }
            //camera backwards
            if (glfwGetKey(window, static_cast<int>(key[Key::CameraBackwards])) == GLFW_PRESS)
            {
                Render::cameraPos -= Render::cameraSpeed * currentSpeed * front;
            }
            //camera left
            if (glfwGetKey(window, static_cast<int>(key[Key::CameraLeft])) == GLFW_PRESS)
            {
                Render::cameraPos -= Render::cameraSpeed * currentSpeed * right;
            }
            //camera right
            if (glfwGetKey(window, static_cast<int>(key[Key::CameraRight])) == GLFW_PRESS)
            {
                Render::cameraPos += Render::cameraSpeed * currentSpeed * right;
            }
            //camera up
            if (glfwGetKey(window, static_cast<int>(key[Key::CameraUp])) == GLFW_PRESS)
            {
                Render::cameraPos += Render::cameraUp * Render::cameraSpeed * currentSpeed;
            }
            //camera down
            if (glfwGetKey(window, static_cast<int>(key[Key::CameraDown])) == GLFW_PRESS)
            {
                Render::cameraPos -= Render::cameraUp * Render::cameraSpeed * currentSpeed;
            }
        }
    }

    void Input::ProcessMouseMovement(double xpos, double ypos) 
    {
        if (mouseFocused)
        {
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

            if (yaw >= 360.00f
                || yaw <= -360.00f)
            {
                yaw = 0.0f;
            }

            if (pitch > 90.0f) pitch = 90.0f;
            if (pitch < -90.0f) pitch = -90.0f;

            vec3 front{};
            front.x = cos(radians(yaw)) * cos(radians(pitch));
            front.y = sin(radians(pitch));
            front.z = sin(radians(yaw)) * cos(radians(pitch));
            cameraFront = normalize(front);
        }
    }

    void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
    {}

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
        if (action == GLFW_PRESS
            && key == GLFW_KEY_ESCAPE)
        {
            mouseFocused = !mouseFocused;
            if (mouseFocused)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }

    void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {}

    mat4 Input::GetViewMatrix() const
    {
        return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
}