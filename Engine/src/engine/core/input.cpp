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

//external
#include "glad.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "magic_enum.hpp"

//engine
#include "console.hpp"
#include "gui.hpp"
#include "render.hpp"
#include "timeManager.hpp"
#include "stringUtils.hpp"
#include "input.hpp"
#include "selectobject.hpp"
#include "gameobject.hpp"

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using glm::radians;
using glm::lookAt;
using std::ostringstream;
using std::fixed;
using std::setprecision;
using std::to_string;
using std::numeric_limits;

using Physics::Select;
using Graphics::Render;
using Core::ConsoleManager;
using Graphics::Shape::GameObjectManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
    void Input::InputSetup()
    {
        ImGuiIO& io = ImGui::GetIO();
    }

    Input::Input(GLFWwindow* window, float sensitivity) : 
        window(window), 
        yaw(-90.0f), 
        pitch(0.0f), 
        lastX(0.0), lastY(0.0), 
        firstMouse(true),
        sensitivity(sensitivity),
        cameraPos(vec3(0.0f, 1.0f, 0.0f)), 
        cameraFront(vec3(0.0f, 0.0f, -1.0f)), 
        cameraUp(vec3(0.0f, 1.0f, 0.0f)) {}

	void Input::ProcessInput(GLFWwindow* window)
	{
        Input::ProcessKeyboardInput(Render::window);

        if (inputSettings.cameraEnabled)
        {
            glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
            {
                Render::camera.ProcessMouseMovement(xpos, ypos);
            });
            inputSettings.cameraSpeed = static_cast<float>(2.5f * TimeManager::deltaTime);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) 
            {
                ImGui::GetIO().MousePos = ImVec2(xpos, ypos);
            });

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        if (inputSettings.printInputToConsole)
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS 
                && !printCheck)
            {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                ostringstream messageStream;
                messageStream 
                    << "Left mouse button pressed at (" 
                    << fixed 
                    << setprecision(2)
                    << mouseX 
                    << ", " 
                    << mouseY 
                    << ")\n";

                ConsoleManager::WriteConsoleMessage(Caller::INPUT, Type::DEBUG, messageStream.str());

                printCheck = true;
            }
            else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            {
                printCheck = false;
            }
        }
	}

    void Input::ProcessKeyboardInput(GLFWwindow* window)
    {
        if (inputSettings.cameraEnabled)
        {
            bool isLeftShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            float currentSpeed = inputSettings.cameraSpeed;
            if (isLeftShiftPressed) currentSpeed = 2.0f * inputSettings.moveSpeedMultiplier;
            else                    currentSpeed = 1.0f * inputSettings.moveSpeedMultiplier;

            vec3 front = Render::camera.GetFront();
            vec3 right = Render::camera.GetRight();

            //camera forwards
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos += inputSettings.cameraSpeed * currentSpeed * front);
            }
            //camera backwards
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos -= inputSettings.cameraSpeed * currentSpeed * front);
            }
            //camera left
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos -= inputSettings.cameraSpeed * currentSpeed * right);
            }
            //camera right
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos += inputSettings.cameraSpeed * currentSpeed * right);
            }
            //camera up
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos += inputSettings.cameraUp * inputSettings.cameraSpeed * currentSpeed);
            }
            //camera down
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos -= inputSettings.cameraUp * inputSettings.cameraSpeed * currentSpeed);
            }
        }
    }

    void Input::ProcessMouseMovement(double xpos, double ypos) 
    {
        if (inputSettings.cameraEnabled)
        {
            if (firstMouse 
                || inputSettings.cameraModeSwitched)
            {
                firstMouse = false;
                inputSettings.cameraModeSwitched = false;

                SetCameraRotation(inputSettings.lastKnownRotation);

                lastX = xpos;
                lastY = ypos;
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
            && key == GLFW_KEY_ESCAPE
            && !ImGui::GetIO().WantCaptureMouse)
        {
            inputSettings.cameraEnabled = !inputSettings.cameraEnabled;
            inputSettings.cameraModeSwitched = true;

            if (!inputSettings.cameraEnabled)
            {
                inputSettings.lastKnownRotation = Render::camera.GetCameraRotation();
            }
        }
        if (action == GLFW_PRESS
            && key == GLFW_KEY_DELETE)
        {
            Select::isObjectSelected = false;
            shared_ptr<GameObject> selectedObj = Select::selectedObj;
            Select::selectedObj = nullptr;
            GameObjectManager::DestroyGameObject(selectedObj);
        }
    }

    void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT 
            && action == GLFW_PRESS)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(Render::window, &mouseX, &mouseY);

            Select::Ray ray = Select::RayFromMouse(mouseX, mouseY, Render::view, Render::projection);

            if (inputSettings.printSelectRayDirectionToConsole)
            {
                string output =
                    "\nMouse X: " + to_string(mouseX) + ", Mouse Y: " + to_string(mouseY) + "\n" +
                    "Ray Direction: " + to_string(ray.direction.x) + ", " + to_string(ray.direction.y) + ", " + to_string(ray.direction.z) + "\n";
                ConsoleManager::WriteConsoleMessage(
                    Caller::INPUT,
                    Type::DEBUG,
                    output);
            }

            vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
            int index = Select::CheckRayObjectIntersections(ray, objects);

            //if user pressed left mouse button over any imgui window
            if (index == -2)
            {
                if (!Select::isObjectSelected)
                {
                    Select::isObjectSelected;
                }
                if (inputSettings.printSelectRayDirectionToConsole)
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::INPUT,
                        Type::DEBUG,
                        "Hit ImGui content...\n");
                }
            }
            //if user did not press any valid gameobject
            else if (index == -1)
            {
                Select::isObjectSelected = false;
                Select::selectedObj = nullptr;
                if (inputSettings.printSelectRayDirectionToConsole)
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::INPUT,
                        Type::DEBUG,
                        "Did not hit anything...\n");
                }

            }
            else
            {
                Select::selectedObj = objects[index];
                Select::isObjectSelected = true;
                if (inputSettings.printSelectRayDirectionToConsole)
                {
                    string output = "Hit " + Select::selectedObj->GetName() + "!\n";
                    ConsoleManager::WriteConsoleMessage(
                        Caller::INPUT,
                        Type::DEBUG,
                        output);
                }
            }
        }
    }

    mat4 Input::GetViewMatrix() const
    {
        return lookAt(Render::camera.GetCameraPosition(), Render::camera.GetCameraPosition() + Render::camera.GetFront(), cameraUp);
    }
}