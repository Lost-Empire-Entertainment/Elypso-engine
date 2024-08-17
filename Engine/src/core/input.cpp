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
#include "model.hpp"
#include "pointlight.hpp"
#include "spotlight.hpp"
#include "compile.hpp"
#include "fileUtils.hpp"
#include "compile.hpp"

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
using std::filesystem::exists;
using std::filesystem::directory_iterator;

using Physics::Select;
using Graphics::Render;
using Core::ConsoleManager;
using EngineFile::SceneFile;
using EngineFile::ConfigFile;
using Graphics::Shape::GameObjectManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Graphics::Shape::Model;
using Graphics::Shape::Material;
using Graphics::Shape::PointLight;
using Graphics::Shape::SpotLight;
using Core::Compilation;
using Utils::File;
using Core::Compilation;
using Core::Input;

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
        else increment = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 1.0f : 0.1f;
    }

    void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (!Compilation::renderBuildingWindow
            && !ImGui::GetIO().WantCaptureMouse)
        {
            float combinedOffset = increment * static_cast<float>(yoffset);

            if (!Render::camera.cameraEnabled)
            {
                if (Select::selectedObj != nullptr)
                {
                    if (objectAction == ObjectAction::move)
                    {
                        vec3 pos = Select::selectedObj->GetTransform()->GetPosition();
                        if (axis == "X") pos = vec3(pos.x + combinedOffset, pos.y, pos.z);
                        else if (axis == "Y") pos = vec3(pos.x, pos.y + combinedOffset, pos.z);
                        else if (axis == "Z") pos = vec3(pos.x, pos.y, pos.z + combinedOffset);

                        Select::selectedObj->GetTransform()->SetPosition(pos);
                        if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
                    }
                    else if (objectAction == ObjectAction::rotate)
                    {
                        vec3 rot = Select::selectedObj->GetTransform()->GetRotation();
                        if (axis == "X") rot = vec3(rot.x + combinedOffset * 10, rot.y, rot.z);
                        else if (axis == "Y") rot = vec3(rot.x, rot.y + combinedOffset * 10, rot.z);
                        else if (axis == "Z") rot = vec3(rot.x, rot.y, rot.z + combinedOffset * 10);
                        Select::selectedObj->GetTransform()->SetRotation(rot);
                        if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
                    }
                    else if (objectAction == ObjectAction::scale)
                    {
                        vec3 scale = Select::selectedObj->GetTransform()->GetScale();
                        if (axis == "X") scale = vec3(scale.x + combinedOffset, scale.y, scale.z);
                        else if (axis == "Y") scale = vec3(scale.x, scale.y + combinedOffset, scale.z);
                        else if (axis == "Z") scale = vec3(scale.x, scale.y, scale.z + combinedOffset);

                        Select::selectedObj->GetTransform()->SetScale(scale);
                        if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);
                    }
                }
            }
            else if (Render::camera.cameraEnabled)
            {
                float currentSpeed = stof(ConfigFile::GetValue("camera_speedMultiplier"));
                float newSpeed = newSpeed = currentSpeed + currentSpeed * combinedOffset;

                if (newSpeed > 100.0f) newSpeed = 100.0f;
                if (newSpeed < 0.1f) newSpeed = 0.1f;

                ConfigFile::SetValue("camera_speedMultiplier", to_string(newSpeed));
            }
        }
    }

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
        if (!Compilation::renderBuildingWindow
            && !Render::camera.cameraEnabled)
        {
            if (!ImGui::GetIO().WantCaptureMouse)
            {
                //copy selected object
                if (key == GLFW_KEY_C
                    && mods == GLFW_MOD_CONTROL
                    && action == GLFW_PRESS
                    && Select::selectedObj != nullptr)
                {
                    copiedObject = Select::selectedObj;

                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::DEBUG,
                        "Copied file '" + copiedObject->GetName()
                        + "_" + to_string(copiedObject->GetID()) + "'\n");
                }

                //paste selected object
                if (key == GLFW_KEY_V
                    && mods == GLFW_MOD_CONTROL
                    && action == GLFW_PRESS
                    && copiedObject != nullptr)
                {
                    string name = copiedObject->GetName();
                    unsigned int nextID = GameObject::nextID++;

                    //if a regular model was pasted
                    if (copiedObject->GetBasicShape() != nullptr)
                    {
                        string extension;
                        string originPath = Engine::currentGameobjectsPath + "\\" + name;

                        for (const auto& entry : directory_iterator(originPath))
                        {
                            extension = path(entry).extension().string();
                            if (extension == ".fbx"
                                || extension == ".obj"
                                || extension == ".glfw")
                            {
                                originPath += "\\" + name + extension;
                                break;
                            }
                        }
                        string targetPath = File::AddIndex(Engine::currentGameobjectsPath, name, "");
                        string targetName = path(targetPath).stem().string();

                        File::CreateNewFolder(targetPath);
                        string destinationPath = targetPath + "\\" + targetName + extension;
                        File::CopyFileOrFolder(originPath, destinationPath);

                        Model::Initialize(
                            copiedObject->GetTransform()->GetPosition(),
                            copiedObject->GetTransform()->GetRotation(),
                            copiedObject->GetTransform()->GetScale(),
                            copiedObject->GetDirectory(),
                            copiedObject->GetMaterial()->GetShaderName(0),
                            copiedObject->GetMaterial()->GetShaderName(1),
                            copiedObject->GetMaterial()->GetTextureName(Material::TextureType::diffuse),
                            copiedObject->GetMaterial()->GetTextureName(Material::TextureType::specular),
                            copiedObject->GetMaterial()->GetTextureName(Material::TextureType::normal),
                            copiedObject->GetMaterial()->GetTextureName(Material::TextureType::height),
                            copiedObject->GetBasicShape()->GetShininess(),
                            targetName,
                            nextID);
                    }

                    //if a point light was copied
                    else if (copiedObject->GetPointLight() != nullptr)
                    {
                        string targetPath = File::AddIndex(Engine::currentGameobjectsPath, name, "");
                        string targetName = path(targetPath).stem().string();
                        File::CreateNewFolder(targetPath);

                        shared_ptr<GameObject> newPointLight = PointLight::InitializePointLight(
                            copiedObject->GetTransform()->GetPosition(),
                            copiedObject->GetTransform()->GetRotation(),
                            copiedObject->GetTransform()->GetScale(),
                            copiedObject->GetMaterial()->GetShaderName(0),
                            copiedObject->GetMaterial()->GetShaderName(1),
                            copiedObject->GetPointLight()->GetDiffuse(),
                            copiedObject->GetPointLight()->GetIntensity(),
                            copiedObject->GetPointLight()->GetDistance(),
                            targetName,
                            nextID);
                    }

                    //if a spotlight was copied
                    else if (copiedObject->GetSpotLight() != nullptr)
                    {
                        string targetPath = File::AddIndex(Engine::currentGameobjectsPath, name, "");
                        string targetName = path(targetPath).stem().string();
                        File::CreateNewFolder(targetPath);

                        shared_ptr<GameObject> newPointLight = SpotLight::InitializeSpotLight(
                            copiedObject->GetTransform()->GetPosition(),
                            copiedObject->GetTransform()->GetRotation(),
                            copiedObject->GetTransform()->GetScale(),
                            copiedObject->GetMaterial()->GetShaderName(0),
                            copiedObject->GetMaterial()->GetShaderName(1),
                            copiedObject->GetSpotLight()->GetDiffuse(),
                            copiedObject->GetSpotLight()->GetIntensity(),
                            copiedObject->GetSpotLight()->GetDistance(),
                            copiedObject->GetSpotLight()->GetInnerAngle(),
                            copiedObject->GetSpotLight()->GetOuterAngle(),
                            targetName,
                            nextID);
                    }

                    if (!SceneFile::unsavedChanges) Render::SetWindowNameAsUnsaved(true);

                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::DEBUG,
                        "Pasted file " + copiedObject->GetName()
                        + "_" + to_string(copiedObject->GetID()) + "\n");
                }

                //delete selected gameobject
                if (key == GLFW_KEY_DELETE
                    && action == GLFW_PRESS
                    && Select::selectedObj != nullptr)
                {
                    Select::isObjectSelected = false;
                    shared_ptr<GameObject> selectedObj = Select::selectedObj;
                    Select::selectedObj = nullptr;
                    GameObjectManager::DestroyGameObject(selectedObj);
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

            //compile game
            if (key == GLFW_KEY_B
                && mods == GLFW_MOD_CONTROL
                && action == GLFW_PRESS)
            {
                Compilation::Compile();
            }

            //run game
            if (key == GLFW_KEY_R
                && mods == GLFW_MOD_CONTROL
                && action == GLFW_PRESS)
            {
                if (!exists(Engine::gameExePath))
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::EXCEPTION,
                        "Game exe does not exist!\n");
                }
                else
                {
                    SceneFile::SaveScene();

                    //
                    // CREATE NEW GAME DOCUMENTS FOLDER AND PLACE ALL SCENES TO IT
                    //

                    string gameName = path(Engine::gameExePath).stem().string();
                    string myGamesFolder = path(Engine::docsPath).parent_path().string() + "\\My Games";
                    if (!exists(myGamesFolder)) File::CreateNewFolder(myGamesFolder);

                    string gameDocsFolder = myGamesFolder + "\\" + gameName;
                    if (exists(gameDocsFolder)) File::DeleteFileOrfolder(gameDocsFolder + "\\scenes");

                    string scenePath = path(Engine::projectPath).parent_path().string();
                    for (const auto& entry : directory_iterator(path(scenePath)))
                    {
                        string stem = path(entry).stem().string();

                        if (stem != "models"
                            && stem != "textures"
                            && stem != "project")
                        {
                            string origin = path(entry).string();
                            string originFileName = path(entry).filename().string();
                            string target = gameDocsFolder + "\\" + originFileName;

                            File::CopyFileOrFolder(origin, target);
                        }
                    }

                    File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
                }
            }

            //save current scene
            if (key == GLFW_KEY_S
                && mods == GLFW_MOD_CONTROL
                && action == GLFW_PRESS)
            {
                SceneFile::SaveScene();
                ConfigFile::SaveConfigFile();
            }
        }
    }

    void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (!Compilation::renderBuildingWindow
            && !Render::camera.cameraEnabled
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