//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <string>
#include <filesystem>

//external
#include "glad.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "magic_enum.hpp"

//game
#include "gui.hpp"
#include "render.hpp"
#include "timeManager.hpp"
#include "input.hpp"
#include "sceneFile.hpp"
#include "configFile.hpp"
#include "core.hpp"
#include "gui_console.hpp"

using std::cout;

using Graphics::Render;
using GameFile::SceneFile;
using GameFile::ConfigFile;
using Core::Input;
using Core::Game;
using Graphics::GUI::GUIConsole;

namespace Core
{
    float increment;
    static bool startedHolding;

    void Input::ProcessKeyboardInput(GLFWwindow* window)
    {
        Render::camera.cameraEnabled =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS
            && !ImGui::GetIO().WantCaptureMouse;

        if (Render::camera.cameraEnabled
            && !startedHolding)
        {
            startedHolding = true;
            appliedUpdate = false;
        }
        if (!Render::camera.cameraEnabled
            && (startedHolding
                || (!startedHolding
                    && Camera::lastKnownRotation == vec3(0)
                    && Render::camera.GetCameraRotation() != vec3(0))))
        {
            Camera::lastKnownRotation = Render::camera.GetCameraRotation();
            startedHolding = false;
        }

        if (Render::camera.cameraEnabled)
        {
            float moveSpeedMultiplier = stof(ConfigFile::GetValue("camera_speedMultiplier"));

            bool isLeftShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            float currentSpeed = Render::camera.cameraSpeed;
            if (isLeftShiftPressed) currentSpeed = 2.0f * moveSpeedMultiplier;
            else                    currentSpeed = 1.0f * moveSpeedMultiplier;

            vec3 front = Render::camera.GetFront();
            vec3 right = Render::camera.GetRight();

            //camera forwards
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    + Render::camera.cameraSpeed * currentSpeed * front);
            }
            //camera backwards
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    - Render::camera.cameraSpeed * currentSpeed * front);
            }
            //camera left
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    - Render::camera.cameraSpeed * currentSpeed * right);
            }
            //camera right
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    + Render::camera.cameraSpeed * currentSpeed * right);
            }
            //camera up
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    + Render::camera.GetUp() * Render::camera.cameraSpeed * currentSpeed);
            }
            //camera down
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    - Render::camera.GetUp() * Render::camera.cameraSpeed * currentSpeed);
            }
        }
    }

    void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            //float combinedOffset = increment * static_cast<float>(yoffset);
        }
    }

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (!Render::camera.cameraEnabled
            && !ImGui::GetIO().WantCaptureMouse)
        {
            //save current scene
            if (key == GLFW_KEY_S
                && mods == GLFW_MOD_CONTROL
                && action == GLFW_PRESS)
            {
                SceneFile::SaveScene();
                ConfigFile::SaveConfigFile();
            }

            if (key == GLFW_KEY_PAGE_UP
                && action == GLFW_PRESS)
            {
                GUIConsole::renderConsole = !GUIConsole::renderConsole;
            }
        }
    }

    void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
    }

    void Input::MouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
    {
        static bool cursorNormal;

        if (Render::camera.cameraEnabled)
        {
            if (cursorNormal)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursorNormal = false;
            }

            Render::camera.RotateCamera(xpos, ypos);

            Render::camera.cameraSpeed = static_cast<float>(2.5f * TimeManager::deltaTime);
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
}

namespace Graphics
{
    void Camera::RotateCamera(double xpos, double ypos)
    {
        if (Render::camera.cameraEnabled)
        {
            if (!Input::appliedUpdate)
            {
                Render::camera.SetCameraRotation(lastKnownRotation);

                Render::camera.lastX = xpos;
                Render::camera.lastY = ypos;

                Input::appliedUpdate = true;
            }

            double xOffset = xpos - Render::camera.lastX;
            double yOffset = Render::camera.lastY - ypos;

            Render::camera.lastX = xpos;
            Render::camera.lastY = ypos;

            xOffset *= Render::camera.sensitivity;
            yOffset *= Render::camera.sensitivity;

            Render::camera.yaw += static_cast<float>(xOffset);
            Render::camera.pitch += static_cast<float>(yOffset);

            if (Render::camera.yaw > 359.99f
                || Render::camera.yaw < -359.99f)
            {
                Render::camera.yaw = 0.0f;
            }
            if (Render::camera.pitch > 89.99f) Render::camera.pitch = 89.99f;
            if (Render::camera.pitch < -89.99f) Render::camera.pitch = -89.99f;

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