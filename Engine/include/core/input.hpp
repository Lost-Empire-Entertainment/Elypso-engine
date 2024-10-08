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
        static inline bool appliedUpdate;

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

        static void ProcessKeyboardInput(GLFWwindow* window);

        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void MouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
    };
}