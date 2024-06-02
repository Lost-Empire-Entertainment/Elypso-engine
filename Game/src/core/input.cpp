//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>

//external
#include "glad.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "magic_enum.hpp"

//game
#include "render.hpp"
#include "timeManager.hpp"
#include "stringUtils.hpp"
#include "input.hpp"
#include "gameobject.hpp"
#include "sceneFile.hpp"
#include "configFile.hpp"

using std::cout;
using std::endl;
using glm::radians;
using glm::lookAt;
using glm::quat;
using glm::rotate;
using std::ostringstream;
using std::fixed;
using std::setprecision;
using std::to_string;
using std::numeric_limits;

using Graphics::Render;
using GameFile::SceneFile;
using GameFile::ConfigFileManager;
using Graphics::Shape::GameObjectManager;

namespace Core
{
    static bool startedHolding;
    static bool appliedUpdate;

    Input::Input(GLFWwindow* window, float sensitivity) : 
        window(window), 
        yaw(-90.0f), 
        pitch(0.0f), 
        lastX(0.0), lastY(0.0),
        sensitivity(sensitivity),
        cameraPos(vec3(0.0f, 1.0f, 0.0f)), 
        cameraFront(vec3(0.0f, 0.0f, -1.0f)), 
        cameraUp(vec3(0.0f, 1.0f, 0.0f)) {}

    void Input::ProcessKeyboardInput(GLFWwindow* window)
    {
        cameraEnabled =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS
            && !ImGui::GetIO().WantCaptureMouse;

        if (cameraEnabled
            && !startedHolding)
        {
            startedHolding = true;
            appliedUpdate = false;
        }
        if (!cameraEnabled
            && (startedHolding
            || (!startedHolding
            && lastKnownRotation == vec3(0)
            && Render::camera.GetCameraRotation() != vec3(0))))
        {
            lastKnownRotation = Render::camera.GetCameraRotation();
            startedHolding = false;
        }

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
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos += Render::camera.cameraUp * cameraSpeed * currentSpeed);
            }
            //camera down
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.cameraPos -= Render::camera.cameraUp * cameraSpeed * currentSpeed);
            }
        }
        else
        {
            increment = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 1.0f : 0.1f;
        }
    }

    void Input::ProcessMouseMovement(double xpos, double ypos) 
    {
        if (cameraEnabled)
        {
            if (!appliedUpdate)
            {
                SetCameraRotation(lastKnownRotation);

                lastX = xpos;
                lastY = ypos;

                appliedUpdate = true;
            }

            double xOffset = xpos - lastX;
            double yOffset = lastY - ypos;

            lastX = xpos;
            lastY = ypos;

            xOffset *= sensitivity;
            yOffset *= sensitivity;

            yaw += static_cast<float>(xOffset);
            pitch += static_cast<float>(yOffset);

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
    {
        if (!cameraEnabled
            && !ImGui::GetIO().WantCaptureMouse)
        {
        }
    }

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
        if (!cameraEnabled
            && !ImGui::GetIO().WantCaptureMouse)
        {
            //save current scene
            if (key == GLFW_KEY_S
                && mods == GLFW_MOD_CONTROL
                && action == GLFW_PRESS)
            {
                SceneFile::SaveScene();
                ConfigFileManager::SaveConfigFile();
            }
        }
    }

    void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (!cameraEnabled
            && button == GLFW_MOUSE_BUTTON_LEFT 
            && action == GLFW_PRESS)
        {
        }
    }

    void Input::MouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
    {
        static bool cursorNormal;

        if (cameraEnabled)
        {
            if (cursorNormal)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursorNormal = false;
            }

            Render::camera.ProcessMouseMovement(xpos, ypos);

            cameraSpeed = static_cast<float>(2.5f * TimeManager::deltaTime);
        }
        else
        {
            if (!cursorNormal)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursorNormal = true;
            }

            ImGui::GetIO().MousePos = ImVec2(static_cast<float>(xpos), static_cast<float>(ypos));
        }
    }

    mat4 Input::GetViewMatrix() const
    {
        return lookAt(Render::camera.GetCameraPosition(), Render::camera.GetCameraPosition() + Render::camera.GetFront(), cameraUp);
    }
}