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
#include "sceneFile.hpp"

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
using EngineFile::SceneFile;
using Graphics::Shape::GameObjectManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
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

        if (cameraEnabled)
        {
            glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
            {
                Render::camera.ProcessMouseMovement(xpos, ypos);
            });
            cameraSpeed = static_cast<float>(2.5f * TimeManager::deltaTime);

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

        if (printInputToConsole)
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
        if (cameraEnabled)
        {
            bool isLeftShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            float currentSpeed = cameraSpeed;
            if (isLeftShiftPressed) currentSpeed = 2.0f * moveSpeedMultiplier;
            else                    currentSpeed = 1.0f * moveSpeedMultiplier;

            vec3 front = Render::camera.GetFront();
            vec3 right = Render::camera.GetRight();

            //camera forwards
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos += cameraSpeed * currentSpeed * front);
            }
            //camera backwards
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos -= cameraSpeed * currentSpeed * front);
            }
            //camera left
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos -= cameraSpeed * currentSpeed * right);
            }
            //camera right
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos += cameraSpeed * currentSpeed * right);
            }
            //camera up
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos += Render::camera.cameraUp * cameraSpeed * currentSpeed);
            }
            //camera down
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos -= Render::camera.cameraUp * cameraSpeed * currentSpeed);
            }
        }
    }

    void Input::ProcessMouseMovement(double xpos, double ypos) 
    {
        if (cameraEnabled)
        {
            if (firstMouse 
                || cameraModeSwitched)
            {
                firstMouse = false;
                cameraModeSwitched = false;

                SetCameraRotation(lastKnownRotation);

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

            if (yaw > 359.99f
                || yaw < -359.99f)
            {
                yaw = 0.0f;
            }
            if (pitch > 89.99f) pitch = 89.99f;
            if (pitch < -89.99f) pitch = -89.99f;

            vec3 front{};
            front.x = cos(radians(yaw)) * cos(radians(pitch));
            front.y = sin(radians(pitch));
            front.z = sin(radians(yaw)) * cos(radians(pitch));
            cameraFront = normalize(front);
        }
    }

    void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
    {}

    void Input::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        
    }

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
        //toggle camera input on and off
        if (key == GLFW_KEY_ESCAPE
            && action == GLFW_PRESS
            && !ImGui::GetIO().WantCaptureMouse)
        {
            cameraEnabled = !cameraEnabled;
            cameraModeSwitched = true;

            if (!cameraEnabled)
            {
                lastKnownRotation = Render::camera.GetCameraRotation();
            }
        }

        if (!cameraEnabled)
        {
            //delete selected gameobject
            if (key == GLFW_KEY_DELETE
                && action == GLFW_PRESS)
            {
                Select::isObjectSelected = false;
                shared_ptr<GameObject> selectedObj = Select::selectedObj;
                Select::selectedObj = nullptr;
                GameObjectManager::DestroyGameObject(selectedObj);

                if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
            }

            //save current scene
            if (key == GLFW_KEY_S
                && mods == GLFW_MOD_CONTROL
                && action == GLFW_PRESS)
            {
                SceneFile::SaveCurrentScene();
            }

            if (Select::selectedObj != nullptr)
            {
                //switch to X axis
                if (key == GLFW_KEY_X
                    && action == GLFW_PRESS
                    && axis != "X")
                {
                    axis = "X";
                }
                //switch to Y axis
                if (key == GLFW_KEY_Y
                    && action == GLFW_PRESS
                    && axis != "Y")
                {
                    axis = "Y";
                }
                //switch to Z axis
                if (key == GLFW_KEY_Z
                    && action == GLFW_PRESS
                    && axis != "Z")
                {
                    axis = "Z";
                }

                //switch to move action
                if (key == GLFW_KEY_W
                    && action == GLFW_PRESS
                    && objectAction != "move")
                {
                    objectAction = "move";
                }
                //switch to rotate action
                if (key == GLFW_KEY_E
                    && action == GLFW_PRESS
                    && objectAction != "rotate")
                {
                    objectAction = "rotate";
                }
                //switch to scale action
                if (key == GLFW_KEY_R
                    && action == GLFW_PRESS
                    && objectAction != "scale")
                {
                    objectAction = "scale";
                }
            }
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

            if (printSelectRayDirectionToConsole)
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
                if (printSelectRayDirectionToConsole)
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
                if (printSelectRayDirectionToConsole)
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::INPUT,
                        Type::DEBUG,
                        "Did not hit anything...\n");
                }
            }
            else
            {
                if (objects[index] != Select::selectedObj
                    || Select::selectedObj == nullptr)
                {
                    objectAction = "move";
                }

                Select::selectedObj = objects[index];
                Select::isObjectSelected = true;
                if (printSelectRayDirectionToConsole)
                {
                    string output = "Hit " + Select::selectedObj->GetName() + "!\n";
                    ConsoleManager::WriteConsoleMessage(
                        Caller::INPUT,
                        Type::DEBUG,
                        output);
                }
            }
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                leftMouseHeld = true;
                glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
            }
            else if (action == GLFW_RELEASE)
            {
                leftMouseHeld = false;
            }
        }
    }

    mat4 Input::GetViewMatrix() const
    {
        return lookAt(Render::camera.GetCameraPosition(), Render::camera.GetCameraPosition() + Render::camera.GetFront(), cameraUp);
    }
}