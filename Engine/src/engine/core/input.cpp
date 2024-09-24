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
#include "fileUtils.hpp"
#include "StringUtils.h"
#include "camera.hpp"
#if ENGINE_MODE
#include "compile.hpp"
#endif

using std::cout;
using std::endl;
using glm::radians;
using glm::lookAt;
using glm::quat;
using glm::rotate;
using glm::mat4;
using std::ostringstream;
using std::fixed;
using std::setprecision;
using std::to_string;
using std::numeric_limits;
using std::filesystem::exists;
using std::filesystem::directory_iterator;

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
using Utils::File;
using Core::Input;
using Utils::String;
using Core::TimeManager;
using Graphics::Camera;
#if ENGINE_MODE
using Core::Compilation;
#endif

namespace Core
{
    float increment;
    static bool startedHolding;

    void Input::ProcessKeyboardInput(GLFWwindow* window)
    {
    }

    void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
#if ENGINE_MODE
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
        }
#endif
    }

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
#if ENGINE_MODE
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
                    shared_ptr<GameObject> selectedObj = Select::selectedObj;
                    copiedObject.clear();
                    copiedObject["name"] = selectedObj->GetName();
                    copiedObject["id"] = to_string(selectedObj->GetID());
                    copiedObject["pos"] =
                        to_string(selectedObj->GetTransform()->GetPosition().x) + ","
                        + to_string(selectedObj->GetTransform()->GetPosition().y) + ","
                        + to_string(selectedObj->GetTransform()->GetPosition().z);
                    copiedObject["rot"] =
                        to_string(selectedObj->GetTransform()->GetRotation().x) + ","
                        + to_string(selectedObj->GetTransform()->GetRotation().y) + ","
                        + to_string(selectedObj->GetTransform()->GetRotation().z);
                    copiedObject["scale"] =
                        to_string(selectedObj->GetTransform()->GetScale().x) + ","
                        + to_string(selectedObj->GetTransform()->GetScale().y) + ","
                        + to_string(selectedObj->GetTransform()->GetScale().z);
                    copiedObject["vertexShader"] = selectedObj->GetMaterial()->GetShaderName(0);
                    copiedObject["fragmentShader"] = selectedObj->GetMaterial()->GetShaderName(1);

                    if (selectedObj->GetBasicShape() != nullptr)
                    {
                        copiedObject["type"] = "model";

                        copiedObject["directory"] = selectedObj->GetDirectory();

                        copiedObject["diffuseTexture"] = selectedObj->GetMaterial()->GetTextureName(Material::TextureType::diffuse);
                        copiedObject["specularTexture"] = selectedObj->GetMaterial()->GetTextureName(Material::TextureType::specular);
                        copiedObject["normalTexture"] = selectedObj->GetMaterial()->GetTextureName(Material::TextureType::normal);
                        copiedObject["heightTexture"] = selectedObj->GetMaterial()->GetTextureName(Material::TextureType::height);

                        copiedObject["shininess"] = to_string(selectedObj->GetBasicShape()->GetShininess());
                    }

                    else if (selectedObj->GetPointLight() != nullptr)
                    {
                        copiedObject["type"] = "point";

                        copiedObject["diffuse"] =
                            to_string(selectedObj->GetPointLight()->GetDiffuse().x) + ","
                            + to_string(selectedObj->GetPointLight()->GetDiffuse().y) + ","
                            + to_string(selectedObj->GetPointLight()->GetDiffuse().z);
                        copiedObject["intensity"] = to_string(selectedObj->GetPointLight()->GetIntensity());
                        copiedObject["distance"] = to_string(selectedObj->GetPointLight()->GetDistance());
                    }

                    else if (selectedObj->GetSpotLight() != nullptr)
                    {
                        copiedObject["type"] = "spot";

                        copiedObject["diffuse"] =
                            to_string(selectedObj->GetSpotLight()->GetDiffuse().x) + ","
                            + to_string(selectedObj->GetSpotLight()->GetDiffuse().y) + ","
                            + to_string(selectedObj->GetSpotLight()->GetDiffuse().z);
                        copiedObject["intensity"] = to_string(selectedObj->GetSpotLight()->GetIntensity());
                        copiedObject["distance"] = to_string(selectedObj->GetSpotLight()->GetDistance());
                        copiedObject["innerAngle"] = to_string(selectedObj->GetSpotLight()->GetInnerAngle());
                        copiedObject["outerAngle"] = to_string(selectedObj->GetSpotLight()->GetOuterAngle());
                    }

                    selectedObj = nullptr;

                    //copiedObject = Select::selectedObj;

                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::DEBUG,
                        "Copied file '" + copiedObject["name"]
                        + "_" + copiedObject["id"] + "'\n");
                }

                //paste selected object
                if (key == GLFW_KEY_V
                    && mods == GLFW_MOD_CONTROL
                    && action == GLFW_PRESS
                    && copiedObject.size() != 0)
                {
                    string name = copiedObject["name"];
                    unsigned int nextID = GameObject::nextID++;

                    vector<string> posSplit = String::Split(copiedObject["pos"].c_str(), ',');
                    vec3 newPos = Render::camera.GetCameraPosition() + Render::camera.GetFront() * 5.0f;
                    int resultX = static_cast<int>(newPos.x);
                    int resultY = static_cast<int>(newPos.y);
                    int resultZ = static_cast<int>(newPos.z);
                    newPos = vec3(resultX, resultY, resultZ);

                    vector<string> rotSplit = String::Split(copiedObject["rot"].c_str(), ',');
                    vec3 rot = vec3(stof(rotSplit[0]), stof(rotSplit[1]), stof(rotSplit[2]));

                    vector<string> scaleSplit = String::Split(copiedObject["scale"].c_str(), ',');
                    vec3 scale = vec3(stof(scaleSplit[0]), stof(scaleSplit[1]), stof(scaleSplit[2]));

                    //if a regular model was pasted
                    if (copiedObject["type"] == "model")
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

                        string diffTexturePath = copiedObject["diffuseTexture"];
                        if (diffTexturePath != "DEFAULTDIFF")
                        {
                            string diffTextureFile = path(diffTexturePath).filename().string();
                            string diffDestinationPath = targetPath + "\\" + diffTextureFile;

                            File::CopyFileOrFolder(diffTexturePath, diffDestinationPath);
                        }

                        string specTexturePath = copiedObject["specularTexture"];
                        if (specTexturePath != "DEFAULTSPEC")
                        {
                            string specTextureFile = path(specTexturePath).filename().string();
                            string specDestinationPath = targetPath + "\\" + specTextureFile;

                            File::CopyFileOrFolder(specTexturePath, specDestinationPath);
                        }

                        cout << "---- copied\n" << originPath << "\nto\n" << targetPath << "\n";

                        Model::Initialize(
                            newPos,
                            rot,
                            scale,
                            copiedObject["directory"],
                            copiedObject["vertexShader"],
                            copiedObject["fragmentShader"],
                            diffTexturePath,
                            specTexturePath,
                            copiedObject["normalTexture"],
                            copiedObject["heightTexture"],
                            stof(copiedObject["shininess"]),
                            targetName,
                            nextID);
                    }

                    //if a point light was copied
                    else if (copiedObject["type"] == "point")
                    {
                        string targetPath = File::AddIndex(Engine::currentGameobjectsPath, name, "");
                        string targetName = path(targetPath).stem().string();
                        File::CreateNewFolder(targetPath);

                        vector<string> diffSplit = String::Split(copiedObject["diffuse"].c_str(), ',');
                        vec3 diff = vec3(stof(diffSplit[0]), stof(diffSplit[1]), stof(diffSplit[2]));

                        shared_ptr<GameObject> newPointLight = PointLight::InitializePointLight(
                            newPos,
                            rot,
                            scale,
                            copiedObject["vertexShader"],
                            copiedObject["fragmentShader"],
                            diff,
                            stof(copiedObject["intensity"]),
                            stof(copiedObject["distance"]),
                            targetName,
                            nextID);
                    }

                    //if a spotlight was copied
                    else if (copiedObject["type"] == "spot")
                    {
                        string targetPath = File::AddIndex(Engine::currentGameobjectsPath, name, "");
                        string targetName = path(targetPath).stem().string();
                        File::CreateNewFolder(targetPath);

                        vector<string> diffSplit = String::Split(copiedObject["diffuse"].c_str(), ',');
                        vec3 diff = vec3(stof(diffSplit[0]), stof(diffSplit[1]), stof(diffSplit[2]));

                        shared_ptr<GameObject> newPointLight = SpotLight::InitializeSpotLight(
                            newPos,
                            rot,
                            scale,
                            copiedObject["vertexShader"],
                            copiedObject["fragmentShader"],
                            diff,
                            stof(copiedObject["intensity"]),
                            stof(copiedObject["distance"]),
                            stof(copiedObject["innerAngle"]),
                            stof(copiedObject["outerAngle"]),
                            targetName,
                            nextID);
                    }

                    SceneFile::SaveScene();

                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::INFO,
                        "Pasted file " + copiedObject["name"] + "\n");
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
                Compilation::installerType = Compilation::InstallerType::compile;
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

            if (!Render::camera.cameraEnabled)
            {
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
#endif
    }

    void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
#if ENGINE_MODE
#else
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            int renderSizeX, renderSizeY;
            glfwGetFramebufferSize(window, &renderSizeX, &renderSizeY);

            if (x >= 0
                && x <= renderSizeX
                && y >= 0
                && y <= renderSizeY)
            {
                Input::ObjectInteraction(
                    static_cast<float>(renderSizeX),
                    static_cast<float>(renderSizeY),
                    x,
                    y);
            }
        }
#endif
    }

    void Input::MouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
    {
    }

    void Input::SceneWindowInput()
    {
        DragCamera();
        MoveCamera();
        SetCameraSpeed();
    }

    void Input::ObjectInteraction(float width, float height, double posX, double posY)
    {
#if ENGINE_MODE
        if (!Compilation::renderBuildingWindow
            && !Render::camera.cameraEnabled)

        {
            Select::Ray ray = Select::RayFromMouse(
                width,
                height,
                posX,
                posY,
                Render::view,
                Render::projection);

            vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
            int index = Select::CheckRayObjectIntersections(ray, objects);

            //if user did not press any valid gameobject
            if (index == -1)
            {
                Select::isObjectSelected = false;
                Select::selectedObj = nullptr;

                objectAction = ObjectAction::none;
                axis = "";
            }
            else
            {
                Select::selectedObj = objects[index];
                Select::isObjectSelected = true;

                if (objectAction == ObjectAction::none)
                {
                    objectAction = ObjectAction::move;
                }
                if (axis == "") axis = "X";
            }
        }
#endif
    }

    void Input::DragCamera()
    {
#if ENGINE_MODE
        Render::camera.cameraEnabled = ImGui::IsMouseDown(ImGuiMouseButton_Right);

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
            double mouseDeltaX = mouseDelta.x;
            double mouseDeltaY = mouseDelta.y;

            //camera rotation with mouse drag
            if (mouseDeltaX != 0.0f
                || mouseDeltaY != 0.0f)
            {
                float sensitivity = 1.0f;

                //invert Y-axis of mouse delta to work with inverted imgui scene window
                mouseDeltaY = -mouseDeltaY;

                Render::camera.RotateCamera(
                    mouseDeltaX * sensitivity,
                    mouseDeltaY * sensitivity);
            }
        }
#else
        static double lastX = 0.0, lastY = 0.0;
        static bool firstMove = true;

        double currentX, currentY;
        glfwGetCursorPos(Render::window, &currentX, &currentY);

        Render::camera.cameraEnabled =
            glfwGetMouseButton(Render::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

        if (Render::camera.cameraEnabled)
        {
            if (firstMove)
            {
                lastX = currentX;
                lastY = currentY;
                firstMove = false;
            }

            double mouseDeltaX = currentX - lastX;
            double mouseDeltaY = currentY - lastY;

            lastX = currentX;
            lastY = currentY;

            //camera rotation with mouse drag
            if (mouseDeltaX != 0.0f
                || mouseDeltaY != 0.0f)
            {
                float sensitivity = 1.0f;

                //invert Y-axis of mouse delta to work with inverted imgui scene window
                mouseDeltaY = -mouseDeltaY;

                Render::camera.RotateCamera(
                    mouseDeltaX * sensitivity,
                    mouseDeltaY * sensitivity);
            }
        }
        else firstMove = true;
#endif
    }

    void Input::MoveCamera()
    {
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

            bool isLeftShiftPressed = ImGui::IsKeyDown(ImGuiKey_LeftShift);
            float currentSpeed = isLeftShiftPressed
                ? static_cast<float>(2.0f * moveSpeedMultiplier * TimeManager::deltaTime)
                : static_cast<float>(1.0f * moveSpeedMultiplier * TimeManager::deltaTime);

            vec3 front = Render::camera.GetFront();
            vec3 right = Render::camera.GetRight();

            //camera forwards
            if (ImGui::IsKeyDown(ImGuiKey_W))
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    + Render::camera.cameraSpeed * currentSpeed * front);
            }
            //camera backwards
            if (ImGui::IsKeyDown(ImGuiKey_S))
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    - Render::camera.cameraSpeed * currentSpeed * front);
            }
            //camera left
            if (ImGui::IsKeyDown(ImGuiKey_A))
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    - Render::camera.cameraSpeed * currentSpeed * right);
            }
            //camera right
            if (ImGui::IsKeyDown(ImGuiKey_D))
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    + Render::camera.cameraSpeed * currentSpeed * right);
            }
            //camera up
            if (ImGui::IsKeyDown(ImGuiKey_E))
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    + Render::camera.GetUp() * Render::camera.cameraSpeed * currentSpeed);
            }
            //camera down
            if (ImGui::IsKeyDown(ImGuiKey_Q))
            {
                Render::camera.SetCameraPosition(
                    Render::camera.GetCameraPosition()
                    - Render::camera.GetUp() * Render::camera.cameraSpeed * currentSpeed);
            }
        }
        else increment = ImGui::IsKeyDown(ImGuiKey_LeftShift) ? 1.0f : 0.1f;
    }

    void Input::SetCameraSpeed()
    {
        if (Render::camera.cameraEnabled)
        {
            float yoffset = ImGui::GetIO().MouseWheel;
            float combinedOffset = increment * static_cast<float>(yoffset);
            float currentSpeed = stof(ConfigFile::GetValue("camera_speedMultiplier"));
            float newSpeed = currentSpeed + currentSpeed * combinedOffset;

            if (newSpeed > 100.0f) newSpeed = 100.0f;
            if (newSpeed < 0.1f) newSpeed = 0.1f;

            ConfigFile::SetValue("camera_speedMultiplier", to_string(newSpeed));
        }
    }
}

namespace Graphics
{
    void Camera::RotateCamera(double deltaX, double deltaY)
    {
        if (Render::camera.cameraEnabled)
        {
            deltaX *= Render::camera.sensitivity;
            deltaY *= Render::camera.sensitivity;

            Render::camera.yaw += static_cast<float>(deltaX);
            Render::camera.pitch += static_cast<float>(deltaY);

            //clamp yaw and pitch to prevent unnatural rotation
            if (Render::camera.yaw > 359.99f 
                || Render::camera.yaw < -359.99f)
            {
                Render::camera.yaw = 0.0f;
            }
            if (Render::camera.pitch > 89.99f) Render::camera.pitch = 89.99f;
            if (Render::camera.pitch < -89.99f) Render::camera.pitch = -89.99f;

            //update camera front vector based on new yaw and pitch
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