//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <chrono>
#include <fstream>
#include <iostream>

//external
#include "magic_enum.hpp"
#include "glfw3.h"
#include "glm.hpp"
#include "glad.h"

//game
#include "console.hpp"
#include "gui_console.hpp"
#include "core.hpp"
#include "render.hpp"
#include "stringutils.hpp"

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

using Graphics::GUI::GUIConsole;
using Core::Game;
using Graphics::Render;
using Utils::String;

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
        logFile.open(Game::docsPath + "/game_log.txt");
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
            if (Game::startedWindowLoop) GUIConsole::AddTextToConsole(internalMsg);
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
                "help - lists all console commands\n" <<
                "qqq - quits the game\n" <<
                "srm 'int' - sets the render mode (shaded (1), wireframe (2)\n" <<
                "rc - resets the camera back to its original position and rotation\n";

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
            Game::Shutdown();
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
        else
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "'" + command + "' is not a valid command! Use 'help' to list all commands and their valid parameters.\n");
        }
	}
}