//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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
#include "magic_enum.hpp"
#include "glad.h"
#include "glfw3.h"
#include "glm.hpp"

//engine
#include "console.hpp"
#include "searchUtils.hpp"
#include "stringUtils.hpp"
#include "gui.hpp"
#include "core.hpp"
#include "shutdown.hpp"
#include "render.hpp"

#include <ctime>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <system_error>

using std::endl;
using std::cout;
using std::error_code;
using std::errc;
using std::stringstream;
using std::setw;
using std::setfill;
using std::ios;
using std::to_string;
using std::filesystem::remove;
using std::filesystem::exists;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::duration_cast;
using std::chrono::time_point_cast;
using std::chrono::system_clock;
using glm::vec3;

using Graphics::GUI;
using Utils::Search;
using Core::Engine;
using Utils::String;
using Core::ShutdownManager;
using Graphics::Render;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
    std::vector<std::string> ConsoleManager::storedLogs;

    ofstream Logger::logFile;

    string Timestamp::GetCurrentTimestamp()
    {
        auto now = system_clock::now();
        auto now_ms = time_point_cast<milliseconds>(now);

        auto now_c = system_clock::to_time_t(now);
        auto ms = duration_cast<microseconds>(now - now_ms);

        auto tm = *localtime(&now_c);

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

    void Logger::InitializeLogger()
    {
        if (!exists(Engine::docsPath))
        {
            cout << Engine::docsPath << " does not exist!" << endl;
            return;
        }
        
        logFile.open(Engine::docsPath + "/engine_log.txt");
        if (!logFile.is_open())
        {
            error_code ec = make_error_code(static_cast<errc>(errno));

            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                "Failed to open log file! Reason: " +
                ec.message() + "\n\n");
        }
    }
    void Logger::CloseLogger()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void Logger::AddLog(const string& message)
    {
        if (logFile.is_open())
        {
            logFile << message << endl;
        }
    }

    void ConsoleManager::AddLog(const std::string& message)
    {
        storedLogs.push_back(message);
    }

    void ConsoleManager::PrintLogsToBuffer()
    {
        for (const auto& log : storedLogs)
        {
            GUI::GetInstance().textBuffer.appendf("%s", log.c_str());
        }
        storedLogs.clear();
    }

    void ConsoleManager::WriteConsoleMessage(Caller caller, Type type, const string& message, bool onlyMessage)
    {
        string timeStamp = Timestamp::GetCurrentTimestamp();
        string theCaller = string(magic_enum::enum_name(caller));
        string theType = string(magic_enum::enum_name(type));

        string invalidMsg = timeStamp + " " + theType + " is not a valid error type!";

        string validMsg = 
            timeStamp + 
            "[" + theCaller + 
            "_" + theType + 
            "] " + message;

        string logMsg;
        string msg;
        string internalConsoleMsg;

        switch (type)
        {
        default:
            logMsg = invalidMsg;
            msg = invalidMsg;
            internalConsoleMsg = invalidMsg;
            break;
        case Type::CLEANUP:
        case Type::DEBUG:
            logMsg = validMsg;
            if (sendDebugMessages)
            {
                msg = onlyMessage ? message : validMsg;
                internalConsoleMsg = onlyMessage ? message : timeStamp + message;
            }
            break;
        case Type::INFO:
        case Type::SUCCESS:
            logMsg = validMsg;
            msg = onlyMessage ? message : validMsg;
            internalConsoleMsg = onlyMessage ? message : timeStamp + message;
            break;
        case Type::EXCEPTION:
            logMsg = validMsg;
            msg = onlyMessage ? message : validMsg;
            internalConsoleMsg = onlyMessage ? message : timeStamp + message;
            break;
        }

        if (Engine::startedWindowLoop)
        {
            GUI::GetInstance().addedText = internalConsoleMsg;
            GUI::GetInstance().writeToConsole = true;
        }
        else
        {
            ConsoleManager::AddLog(internalConsoleMsg);
        }
        cout << logMsg;
        Logger::AddLog(logMsg);
    }

    void ConsoleManager::ParseConsoleCommand(const string& command)
    {
        if (command == "") 
        {
            return;
        }

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
                "help - lists all console commands\n" <<
                "qqq - quits the engine\n" <<
                "srm 'int' - sets the render mode (shaded (1), wireframe (2)\n" <<
                "rc - resets the camera back to its original position and rotation\n" <<
                "ccc - cleans the console\n";

            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                ss.str(),
                true);
        }
        else if (cleanedCommands[0] == "qqq"
                 && cleanedCommands.size() == 1)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::DEBUG,
                "User closed engine with 'qqq' console command.\n");
            ShutdownManager::Shutdown();
        }
        else if (cleanedCommands[0] == "srm"
                 && (cleanedCommands[1] == "1"
                 || cleanedCommands[1] == "2")
                 && cleanedCommands.size() == 2)
        {
            wireframeMode = cleanedCommands[1] != "1";
            glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);

            string wireframeModeValue = cleanedCommands[1] == "1" ?
                "shaded" :
                "wireframe";
            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Set wireframe mode to " + wireframeModeValue + ".\n");
        }
        else if (cleanedCommands[0] == "rc"
                 && cleanedCommands.size() == 1)
        {
            Render::cameraPos.x = 0;
            Render::cameraPos.y = 0;
            Render::cameraPos.z = 3;
            Render::camera.SetCameraRotation(vec3(-90, 0, 0));

            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Reset camera position and rotation.\n");
        }
        else if (cleanedCommands[0] == "ccc"
                 && cleanedCommands.size() == 1)
        {
            GUI::textBuffer.clear();
        }
        else
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "'" + command + "' is not a valid command! Use 'help' to list all commands and their valid parameters.\n");
        }
    }
}