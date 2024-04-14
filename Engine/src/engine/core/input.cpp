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

using Physics::Select;
using Graphics::Render;
using Core::ConsoleManager;
using EngineFile::SceneFile;
using EngineFile::ConfigFileManager;
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
            float combinedOffset = increment * static_cast<float>(yoffset);

            if (objectAction == ObjectAction::move)
            {
                vec3 pos = Select::selectedObj->GetTransform()->GetPosition();
                if (axis == "X") pos = vec3(pos.x + combinedOffset, pos.y, pos.z);
                else if (axis == "Y") pos = vec3(pos.x, pos.y + combinedOffset, pos.z);
                else if (axis == "Z") pos = vec3(pos.x, pos.y, pos.z + combinedOffset);

                Select::selectedObj->GetTransform()->SetPosition(pos);
            }
            else if (objectAction == ObjectAction::rotate)
            {
                vec3 rot = Select::selectedObj->GetTransform()->GetRotation();
                if (axis == "X") rot = vec3(rot.x + combinedOffset * 10, rot.y, rot.z);
                else if (axis == "Y") rot = vec3(rot.x, rot.y + combinedOffset * 10, rot.z);
                else if (axis == "Z") rot = vec3(rot.x, rot.y, rot.z + combinedOffset * 10);
                Select::selectedObj->GetTransform()->SetRotation(rot);
            }
            else if (objectAction == ObjectAction::scale)
            {
                vec3 scale = Select::selectedObj->GetTransform()->GetScale();
                if (axis == "X") scale = vec3(scale.x + combinedOffset, scale.y, scale.z);
                else if (axis == "Y") scale = vec3(scale.x, scale.y + combinedOffset, scale.z);
                else if (axis == "Z") scale = vec3(scale.x, scale.y, scale.z + combinedOffset);

                Select::selectedObj->GetTransform()->SetScale(scale);
            }
        }
    }

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
        //toggle camera input on and off
        if (key == GLFW_KEY_ESCAPE
            && action == GLFW_PRESS
            && ((!ImGui::GetIO().WantCaptureMouse
            && !cameraEnabled)
            || cameraEnabled))
        {
            cameraEnabled = !cameraEnabled;
            cameraModeSwitched = true;

            if (!cameraEnabled) lastKnownRotation = Render::camera.GetCameraRotation();
        }

        if (!cameraEnabled
            && !ImGui::GetIO().WantCaptureMouse)
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
                SceneFile::SaveScene();
                ConfigFileManager::SaveConfigFile();
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
                    && objectAction != ObjectAction::move)
                {
                    objectAction = ObjectAction::move;
                }
                //switch to rotate action
                if (key == GLFW_KEY_E
                    && action == GLFW_PRESS
                    && objectAction != ObjectAction::rotate)
                {
                    objectAction = ObjectAction::rotate;
                }
                //switch to scale action
                if (key == GLFW_KEY_R
                    && action == GLFW_PRESS
                    && objectAction != ObjectAction::scale)
                {
                    objectAction = ObjectAction::scale;
                }
            }
        }
    }

    void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (!cameraEnabled
            && button == GLFW_MOUSE_BUTTON_LEFT 
            && action == GLFW_PRESS)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(Render::window, &mouseX, &mouseY);

            Select::Ray ray = Select::RayFromMouse(mouseX, mouseY, Render::view, Render::projection);

            vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
            int index = Select::CheckRayObjectIntersections(ray, objects);

            //if user pressed left mouse button over any imgui window
            if (index == -2)
            {
                if (!Select::isObjectSelected)
                {
                    Select::isObjectSelected;
                }
            }
            //if user did not press any valid gameobject
            else if (index == -1)
            {
                Select::isObjectSelected = false;
                Select::selectedObj = nullptr;
            }
            else
            {
                if (objects[index] != Select::selectedObj
                    || Select::selectedObj == nullptr)
                {
                    objectAction = ObjectAction::none;
                }

                Select::selectedObj = objects[index];
                Select::isObjectSelected = true;
                Input::objectAction = Input::ObjectAction::move;
            }
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