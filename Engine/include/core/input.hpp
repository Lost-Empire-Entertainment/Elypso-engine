//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <unordered_map>
#include <string>
#include <memory>

//external
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //two matrix_transform.hpp files exist in glm
#include "glfw3.h"

//engine
#include "input.hpp"
#include "gameobject.hpp"

using glm::vec3;
using glm::mat4;
using glm::radians;
using std::unordered_map;
using std::string;
using std::shared_ptr;

using Graphics::Shape::GameObject;

namespace Core
{
    class Input
    {
    public:
        static inline vec3 lastKnownRotation = vec3(0);
        static inline float cameraSpeed = 2.5f;
        static inline float aspectRatio = 0.0f;
        static inline bool cameraEnabled;

        static inline string axis = "X";
        enum class ObjectAction
        {
            none,
            move,
            rotate,
            scale
        };
        static inline ObjectAction objectAction = ObjectAction::none;

        static inline shared_ptr<GameObject> copiedObject;

        Input(GLFWwindow* window, float sensitivity = 0.05f);
        mat4 GetViewMatrix() const;

        static void ProcessKeyboardInput(GLFWwindow* window);

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
        float sensitivity;

        vec3 cameraPos;
        vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
        vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

        void ProcessMouseMovement(double xpos, double ypos);
    };
}