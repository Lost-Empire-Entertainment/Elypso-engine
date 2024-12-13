//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <chrono>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <memory>
#include <filesystem>

//external
#include "glad.h"
#include "magic_enum.hpp"
#include "glfw3.h"
#include "glm.hpp"

//engine
#include "console.hpp"
#include "core.hpp"
#include "render.hpp"
#include "stringUtils.hpp"
#include "selectobject.hpp"
#include "gameobject.hpp"
#include "gui_console.hpp"
#if ENGINE_MODE
#include "gui_engine.hpp"
#endif

using std::chrono::time_point_cast;
using std::chrono::system_clock;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::stringstream;
using std::setw;
using std::setfill;
using std::ofstream;
using std::error_code;
using std::errc;
using std::cout;
using glm::vec3;
using std::filesystem::path;
using std::filesystem::current_path;

using Core::Engine;
using Graphics::Render;
using Utils::String;
using Core::Select;
using Graphics::Shape::GameObject;
using Graphics::Shape::Mesh;
using Graphics::Shape::Material;
using Graphics::GUI::GUIConsole;
#if ENGINE_MODE
using Graphics::GUI::EngineGUI;
#endif

namespace Core
{
    ofstream logFile;

    string ConsoleManager::GetCurrentTimestamp()
    {
        auto now = system_clock::now();
        auto now_ms = time_point_cast<milliseconds>(now);

        auto now_c = system_clock::to_time_t(now);
        auto ms = duration_cast<milliseconds>(now_ms.time_since_epoch()) % 1000;

        tm tm;
#ifdef _WIN32
        localtime_s(&tm, &now_c);
#elif __linux__
        localtime_r(&now_c, &tm);
#endif

        stringstream ss{};
        ss
            << "["
            << setw(2) << setfill('0') << tm.tm_hour << ":"
            << setw(2) << setfill('0') << tm.tm_min << ":"
            << setw(2) << setfill('0') << tm.tm_sec << ":"
            << setw(3) << setfill('0') << ms.count()
            << "] ";
        return ss.str();
    }

    void ConsoleManager::InitializeLogger()
    {
#if ENGINE_MODE
        logFile.open((path(Engine::docsPath) / "engine_log.txt").string());
#else
        path currentPath = current_path();
        logFile.open((currentPath / "game_log.txt").string());
#endif
        if (!logFile.is_open())
        {
            error_code ec = make_error_code(static_cast<errc>(errno));

            WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to open log file! Reason: " +
                ec.message() + "\n\n");
        }
    }

    void ConsoleManager::PrintLogsToBuffer()
    {
        for (const auto& log : storedLogs)
        {
            GUIConsole::AddTextToConsole(log);
        }
        storedLogs.clear();
    }

    void ConsoleManager::AddLoggerLog(const string& message)
    {
        if (logFile.is_open())
        {
            logFile << message << "\n";
        }
    }

    void ConsoleManager::AddConsoleLog(const std::string& message)
    {
        storedLogs.push_back(message);
    }

    void ConsoleManager::CloseLogger()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void ConsoleManager::WriteConsoleMessage(Caller caller, Type type, const string& message, bool onlyMessage, bool internalMessage)
    {
        string timeStamp = GetCurrentTimestamp();
        string theCaller = string(magic_enum::enum_name(caller));
        string theType = string(magic_enum::enum_name(type));

        string invalidMsg = timeStamp + " " + theType + " is not a valid error type!";

        string validMsg =
            timeStamp +
            "[" + theCaller +
            "_" + theType +
            "] " + message;

        string externalMsg;
        string internalMsg;

        switch (type)
        {
        default:
            externalMsg = invalidMsg;
            internalMsg = invalidMsg;
            break;
        case Type::DEBUG:
            if (sendDebugMessages) internalMsg = onlyMessage ? message : timeStamp + message;
            externalMsg = onlyMessage ? message : validMsg;
            break;
        case Type::INFO:
            externalMsg = onlyMessage ? message : validMsg;
            internalMsg = onlyMessage ? message : timeStamp + message;
            break;
        case Type::EXCEPTION:
            externalMsg = onlyMessage ? message : validMsg;
            internalMsg = onlyMessage ? message : timeStamp + message;
            break;
        }

        if (internalMessage
            && (sendDebugMessages
            || (!sendDebugMessages
            && type != Type::DEBUG)))
        {
            if (Engine::isEngineRunning) GUIConsole::AddTextToConsole(internalMsg);
            else AddConsoleLog(internalMsg);
        }

        cout << externalMsg;
        AddLoggerLog(externalMsg);
    }

    void ConsoleManager::ParseConsoleCommand(const string& command)
    {
        if (command == "") return;

        vector<string> splitCommand = String::Split(command, ' ');
        size_t count = splitCommand.size();

        vector<string> cleanedCommands;
        for (const auto& part : splitCommand)
        {
            string partWithoutSpaces = part;
            partWithoutSpaces.erase(
                remove_if(partWithoutSpaces.begin(), partWithoutSpaces.end(), ::isspace),
                partWithoutSpaces.end());

            if (!partWithoutSpaces.empty())
            {
                cleanedCommands.push_back(partWithoutSpaces);
            }
        }

        if (cleanedCommands[0] == "help"
            && cleanedCommands.size() == 1)
        {
            stringstream ss;
            ss <<
                "help - lists all console commands\n"
                << "qqq - quits the engine\n"
                << "srm 'int' - sets the render mode (shaded (1), wireframe (2)\n"
                << "rc - resets the camera back to its original position and rotation\n"
                << "dbg - prints debug info about selected gameobject (click on object before using this command)"
#if ENGINE_MODE
#else
                << "toggle - enables or disables selected gameobject based on its enabled state (click on object before using this command)"
#endif
                ;

            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                ss.str(),
                true);
        }
        else if (cleanedCommands[0] == "qqq"
                 && cleanedCommands.size() == 1)
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::DEBUG,
                "User closed game with 'qqq' console command.\n");
            Engine::Shutdown();
        }
        else if (cleanedCommands[0] == "rc"
                 && cleanedCommands.size() == 1)
        {
            vec3 newPosition = vec3(0.0f, 1.0f, 0.0f);
            Render::camera.SetCameraPosition(newPosition);
            Render::camera.SetCameraRotation(vec3(-90, 0, 0));

            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Reset camera position and rotation.\n");
        }
        else if (cleanedCommands[0] == "dbg"
                 && cleanedCommands.size() == 1)
        {
            if (Select::selectedObj == nullptr)
            {
                WriteConsoleMessage(
                    Caller::INPUT,
                    Type::EXCEPTION,
                    "Error: Please select a gameobject first before using the 'dbg' command.\n");
            }
            else
            {
                PrintSelectObjectData();
            }
        }
#if ENGINE_MODE
#else
        else if (cleanedCommands[0] == "toggle"
                 && cleanedCommands.size() == 1)
        {
            if (Select::selectedObj == nullptr)
            {
                WriteConsoleMessage(
                    Caller::INPUT,
                    Type::EXCEPTION,
                    "Please select a gameobject first before using the 'toggle' command.\n");
            }
            else
            {
                bool enabled = Select::selectedObj->IsEnabled();
                enabled = !enabled;
                Select::selectedObj->SetEnableState(enabled);
            }
        }
#endif
        else if (cleanedCommands[0] == "srm"
                 && (cleanedCommands[1] == "1"
                 || cleanedCommands[1] == "2")
                 && cleanedCommands.size() == 2)
        {
            wireframeMode = cleanedCommands[1] != "1";
            glPolygonMode(
                GL_FRONT_AND_BACK,
                wireframeMode ? GL_LINE : GL_FILL);

            string wireframeModeValue = cleanedCommands[1] == "1" ?
                "shaded" :
                "wireframe";
            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Set wireframe mode to " + wireframeModeValue + ".\n");
        }
        else
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "Error: '" + command + "' is not a valid command! Use 'help' to list all commands and their valid parameters.\n");
        }
    }

    void ConsoleManager::PrintSelectObjectData()
    {
        shared_ptr<GameObject> obj = Select::selectedObj;

        stringstream ss;

        ss << "\n--------------------\n"
            << "name: " << obj->GetName() << "\n"
            << "id: " << obj->GetID() << "\n"
            << "is enabled: " << obj->IsEnabled() << "\n"

            << "position: " << obj->GetTransform()->GetPosition().x << ", "
            << obj->GetTransform()->GetPosition().y << ", "
            << obj->GetTransform()->GetPosition().z << "\n"

            << "rotation: " << obj->GetTransform()->GetRotation().x << ", "
            << obj->GetTransform()->GetRotation().y << ", "
            << obj->GetTransform()->GetRotation().z << "\n"

            << "scale: " << obj->GetTransform()->GetScale().x << ", "
            << obj->GetTransform()->GetScale().y << ", "
            << obj->GetTransform()->GetScale().z << "\n"

            << "mesh type: " << magic_enum::enum_name(obj->GetMesh()->GetMeshType()) << "\n"
            << "--------------------\n";

        if (obj->GetMesh()->GetMeshType() == Mesh::MeshType::model)
        {
            ss << "model shininess: 32\n";
        }
        else if (obj->GetMesh()->GetMeshType() == Mesh::MeshType::point_light)
        {
            ss << "point light diffuse: "
                << obj->GetPointLight()->GetDiffuse().x << ", "
                << obj->GetPointLight()->GetDiffuse().y << ", "
                << obj->GetPointLight()->GetDiffuse().z << "\n"

                << "point light intensity: " << obj->GetPointLight()->GetIntensity() << "\n"

                << "point light distance: " << obj->GetPointLight()->GetDistance() << "\n";
        }
        else if (obj->GetMesh()->GetMeshType() == Mesh::MeshType::spot_light)
        {
            ss << "spotlight diffuse: "
                << obj->GetSpotLight()->GetDiffuse().x << ", "
                << obj->GetSpotLight()->GetDiffuse().y << ", "
                << obj->GetSpotLight()->GetDiffuse().z << "\n"

                << "spotlight intensity: " << obj->GetSpotLight()->GetIntensity() << "\n"

                << "spotlight distance: " << obj->GetSpotLight()->GetDistance() << "\n"

                << "spotlight outer angle: " << obj->GetSpotLight()->GetOuterAngle() << "\n"

                << "spotlight inner angle: " << obj->GetSpotLight()->GetInnerAngle() << "\n";
        }
        else if (obj->GetMesh()->GetMeshType() == Mesh::MeshType::directional_light)
        {
            ss << "directional light diffuse: "
                << obj->GetDirectionalLight()->GetDiffuse().x << ", "
                << obj->GetDirectionalLight()->GetDiffuse().y << ", "
                << obj->GetDirectionalLight()->GetDiffuse().z << "\n"

                << "directional light intensity: " << obj->GetDirectionalLight()->GetIntensity() << "\n";
        }

        ss << "--------------------\n";

        WriteConsoleMessage(
            Caller::INPUT,
            Type::INFO,
            ss.str());
    }
}