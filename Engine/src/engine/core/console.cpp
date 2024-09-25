//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>

//external
#include "magic_enum.hpp"
#include "glfw3.h"
#include "glm.hpp"
#include "glad.h"

//engine
#include "console.hpp"
#include "core.hpp"
#include "render.hpp"
#include "stringutils.hpp"
#include "selectobject.hpp"
#include "gameobject.hpp"
#if ENGINE_MODE
#include "gui_engine.hpp"
#include "gui_console.hpp"
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

using Core::Engine;
using Graphics::Render;
using Utils::String;
using Core::Select;
using Graphics::Shape::GameObject;
using Graphics::Shape::Mesh;
using Graphics::Shape::Material;
#if ENGINE_MODE
using Graphics::GUI::EngineGUI;
using Graphics::GUI::GUIConsole;
#endif

namespace Core
{
    ofstream logFile;

    string ConsoleManager::GetCurrentTimestamp()
    {
        auto now = system_clock::now();
        auto now_ms = time_point_cast<milliseconds>(now);

        auto now_c = system_clock::to_time_t(now);
        auto ms = duration_cast<microseconds>(now - now_ms);

        tm tm;
        localtime_s(&tm, &now_c);

        stringstream ss{};
        ss
            << "["
            << setw(2) << setfill('0') << tm.tm_hour << ":"
            << setw(2) << setfill('0') << tm.tm_min << ":"
            << setw(2) << setfill('0') << tm.tm_sec << ":"
            << setw(3) << ms.count()
            << "] ";
        return ss.str();
    }

    void ConsoleManager::InitializeLogger()
    {
        logFile.open(Engine::docsPath + "/engine_log.txt");
        if (!logFile.is_open())
        {
            error_code ec = make_error_code(static_cast<errc>(errno));

            WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Failed to open log file! Reason: " +
                ec.message() + "\n\n");
        }
    }

    void ConsoleManager::PrintLogsToBuffer()
    {
        for (const auto& log : storedLogs)
        {
#if ENGINE_MODE
            GUIConsole::AddTextToConsole(log);
#endif
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
#if ENGINE_MODE
            if (Engine::startedWindowLoop) GUIConsole::AddTextToConsole(internalMsg);
            else AddConsoleLog(internalMsg);
#endif
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
                << "dbg - prints debug info about selected gameobject (click on object before using this command)"
                << "srm 'int' - sets the render mode (shaded (1), wireframe (2)\n"
                << "rc - resets the camera back to its original position and rotation\n"
                << "saw - toggles between showing and not showing imgui about window\n"
                << "sdlw - toggles between showing and not showing imgui debug log window\n"
                << "sdw - toggles between showing and not showing imgui demo window\n"
                << "sisw - toggles between showing and not showing imgui id stack window\n"
                << "smw - toggles between showing and not showing imgui metrics window\n"
                << "sstw - toggles between showing and not showing imgui stack tool window\n"
                << "ssew - toggles between showing and not showing imgui style editor window\n"
                << "sugw - toggles between showing and not showing imgui user guide window\n";

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
        else if (cleanedCommands[0] == "dbg"
                 && cleanedCommands.size() == 1)
        {
            if (Select::selectedObj == nullptr)
            {
                WriteConsoleMessage(
                    Caller::INPUT,
                    Type::EXCEPTION,
                    "Please select a gameobject first before using the 'dbg' command.\n");
            }
            else
            {
                PrintSelectObjectData();
            }
        }
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

#if ENGINE_MODE
        else if (cleanedCommands[0] == "saw"
                 && cleanedCommands.size() == 1)
        {
            EngineGUI::renderAboutWindow = !EngineGUI::renderAboutWindow;

            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Set show about window to " + to_string(EngineGUI::renderAboutWindow) + "\n");
        }
        else if (cleanedCommands[0] == "sdlw"
                 && cleanedCommands.size() == 1)
        {
            EngineGUI::renderDebugLogWindow = !EngineGUI::renderDebugLogWindow;

            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Set show debug log window to " + to_string(EngineGUI::renderDebugLogWindow) + "\n");
        }
        else if (cleanedCommands[0] == "sdw"
                 && cleanedCommands.size() == 1)
        {
            EngineGUI::renderDemoWindow = !EngineGUI::renderDemoWindow;

            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Set show demo window to " + to_string(EngineGUI::renderDemoWindow) + "\n");
        }
        else if (cleanedCommands[0] == "sisw"
                 && cleanedCommands.size() == 1)
        {
            EngineGUI::renderIDStackWindow = !EngineGUI::renderIDStackWindow;

            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Set show id stack window to " + to_string(EngineGUI::renderIDStackWindow) + "\n");
        }
        else if (cleanedCommands[0] == "smw"
                 && cleanedCommands.size() == 1)
        {
            EngineGUI::renderMetricsWindow = !EngineGUI::renderMetricsWindow;

            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Set show metrics window to " + to_string(EngineGUI::renderMetricsWindow) + "\n");
        }
        else if (cleanedCommands[0] == "sstw"
                 && cleanedCommands.size() == 1)
        {
                EngineGUI::renderStackToolWindow = !EngineGUI::renderStackToolWindow;

                WriteConsoleMessage(
                    Caller::INPUT,
                    Type::INFO,
                    "Set show stack tools window to " + to_string(EngineGUI::renderStackToolWindow) + "\n");
        }
        else if (cleanedCommands[0] == "ssew"
                 && cleanedCommands.size() == 1)
        {
                EngineGUI::renderStyleEditorWindow = !EngineGUI::renderStyleEditorWindow;

                WriteConsoleMessage(
                    Caller::INPUT,
                    Type::INFO,
                    "Set show style editor window to " + to_string(EngineGUI::renderStyleEditorWindow) + "\n");
        }
        else if (cleanedCommands[0] == "sugw"
                 && cleanedCommands.size() == 1)
        {
                EngineGUI::renderUserGuideWindow = !EngineGUI::renderUserGuideWindow;

                WriteConsoleMessage(
                    Caller::INPUT,
                    Type::INFO,
                    "Set show user guide window to " + to_string(EngineGUI::renderUserGuideWindow) + "\n");
        }
#endif

        else
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "'" + command + "' is not a valid command! Use 'help' to list all commands and their valid parameters.\n");
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
            ss << "model shininess: " << obj->GetBasicShape()->GetShininess() << "\n";
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