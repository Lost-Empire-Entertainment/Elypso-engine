//Copyright(C) 2025 Lost Empire Entertainment
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
#include "selectobject.hpp"
#include "gameobject.hpp"
#include "gui_console.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lightcomponent.hpp"
#include "transformcomponent.hpp"
#include "stringutils.hpp"
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
using std::to_string;
using std::size_t;
using std::stoi;

using Core::Engine;
using Graphics::Render;
using Core::Select;
using Graphics::Shape::GameObject;
using Graphics::Components::MeshComponent;
using Graphics::Components::MaterialComponent;
using Graphics::Components::LightComponent;
using Graphics::GUI::GUIConsole;
using Graphics::Components::TransformComponent;
using Utils::String;
#if ENGINE_MODE
using Graphics::GUI::EngineGUI;
#endif

namespace Core
{
    ofstream logFile;

    static bool StringToInt(const string& in, int& out)
    {
        try
        {
            size_t pos{};
            out = stoi(in, &pos);
            return pos == in.length();
        }
        catch (...)
        {
            return false;
        }
    }

    void ConsoleManager::AddCommands()
    {
        AddCommand("help", "Prints all console command names.", 1, CommandTarget::TARGET_BOTH, Command_Help_All);

        string hDesc = "Prints info about the selected command.";
        AddCommand("h", hDesc, 2, CommandTarget::TARGET_BOTH, Command_Help_Info);

        string srmDesc = "Sets the OpenGL render mode. 1 = Shaded, 2 = Wireframe.";
        AddCommand("srm", srmDesc, 2, CommandTarget::TARGET_BOTH, Command_SetRenderMode);

        string qqqDesc = "Force-quits the engine or game, bypassing all checks and quickly saving the most important things.";
        AddCommand("qqq", qqqDesc, 1, CommandTarget::TARGET_BOTH, Command_Quit);

#if ENGINE_MODE

        string rcDesc = "Resets the scene camera position to (0, 1, 0) and rotation to (0, 0, 0).";
        AddCommand("rc", rcDesc, 1, CommandTarget::TARGET_ENGINE, Command_Engine_ResetCamera);

#endif
    }

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
        if (logFile.is_open()) logFile << message << "\n";
    }

    void ConsoleManager::AddConsoleLog(const std::string& message)
    {
        storedLogs.push_back(message);
    }

    void ConsoleManager::CloseLogger()
    {
        if (logFile.is_open()) logFile.close();
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
            if (Engine::isRunning) GUIConsole::AddTextToConsole(internalMsg);
            else AddConsoleLog(internalMsg);
        }

        cout << externalMsg;
        AddLoggerLog(externalMsg);
    }

    void ConsoleManager::AddCommand(
        const string& command,
        const string& description,
        unsigned int parameterCount,
        CommandTarget target,
        function<void(const vector<string>&)> action)
    {
        if (parameterCount == 0)
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "Error: Cannot create command '" + command + "' with parameter count '0'! Parameter count must atleast be 1 or higher.\n");

            return;
        }

        Command cmd
        {
            description,
            parameterCount,
            target,
            action
        };

        if (target == CommandTarget::TARGET_ENGINE)
        {
#if ENGINE_MODE
            engineCommands[command] = cmd;
#endif
        }
        else if (target == CommandTarget::TARGET_GAME)
        {
            gameCommands[command] = cmd;
        }
        else
        {
#if ENGINE_MODE
            engineCommands[command] = cmd;
#else
            WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "Error: Cannot pass console commands to engine in game!\n");
#endif
            gameCommands[command] = cmd;
        }
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

        FindCommand(splitCommand[0], splitCommand);
    }

    void ConsoleManager::FindCommand(
        const string& name, 
        const vector<string>& parameters)
    {
        Command* foundCommand = nullptr;

#if ENGINE_MODE
        auto it = engineCommands.find(name);
        if (it != engineCommands.end())
        {
            foundCommand = &it->second;
        }
#else
        auto it = gameCommands.find(name);
        if (it != gameCommands.end())
        {
            foundCommand = &it->second;
        }
#endif

        if (foundCommand == nullptr)
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "Error: Command with name '" 
                + name 
                + "' does not exist! Type 'help' to list all commands.\n");
            return;
        }

        if (parameters.size() != foundCommand->parameterCount)
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "Error: Command '" 
                + name 
                + "' requires " 
                + to_string(foundCommand->parameterCount) 
                + " parameter(s), but got " + to_string(parameters.size())
                + ". Type 'help' to list all commands, or 'h " 
                + name 
                + "' to view details about this command.\n");
            return;
        }

        foundCommand->action(parameters);
    }

    //
    // ALL CONSOLE COMMANDS USED IN BOTH ENGINE AND GAME
    //

    void ConsoleManager::Command_Help_All(const vector<string>& args)
    {
        WriteConsoleMessage(
            Caller::INPUT,
            Type::INFO,
            "Listing all console commands:\n",
            true);

#if ENGINE_MODE
        for (const auto& [name, cmd] : engineCommands)
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                name + "\n",
                true);
        }
#else
        for (const auto& [name, cmd] : gameCommands)
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                name + "\n",
                true);
        }
#endif
    }

    void ConsoleManager::Command_Help_Info(const vector<string>& args)
    {
        Command* targetCommand{};

#if ENGINE_MODE
        auto it = engineCommands.find(args[1]);
        if (it != engineCommands.end())
        {
            targetCommand = &it->second;
        }
#else
        auto it = gameCommands.find(args[1]);
        if (it != gameCommands.end())
        {
            targetCommand = &it->second;
        }
#endif

        if (targetCommand == nullptr)
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::EXCEPTION,
                "Error: Did not find command with name '" + args[1] + "'! Type 'help' to list all commands.\n");

            return;
        }

        WriteConsoleMessage(
            Caller::INPUT,
            Type::INFO,
            targetCommand->description + "\n",
            true);
    }

    void ConsoleManager::Command_SetRenderMode(const vector<string>& args)
    {
        int choice{};

        if (!StringToInt(args[1], choice))
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Error: '" + args[1] + "' is not an integer! Type 'h srm' to get more info about this command.\n");

            return;
        }

        if (choice != 1
            && choice != 2)
        {
            WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Error: '" + args[1] + "' is out of range! Type 'h srm' to get more info about this command.\n");

            return;
        }

        wireframeMode = args[1] != "1";
        glPolygonMode(
            GL_FRONT_AND_BACK,
            wireframeMode ? GL_LINE : GL_FILL);

        string wireframeModeValue = choice == 1 ?
            "shaded" :
            "wireframe";
        WriteConsoleMessage(
            Caller::INPUT,
            Type::INFO,
            "Set wireframe mode to " + wireframeModeValue + ".\n");
    }

    void ConsoleManager::Command_Quit(const vector<string>& args)
    {
        WriteConsoleMessage(
            Caller::INPUT,
            Type::DEBUG,
            "User closed engine with 'qqq' console command.\n");
        Engine::Shutdown(true, true);
    }

#if ENGINE_MODE

    //
    // ALL CONSOLE COMMANDS IN ENGINE
    //

    void ConsoleManager::Command_Engine_ResetCamera(const vector<string>& args)
    {
        vec3 newPosition = vec3(0.0f, 1.0f, 0.0f);
        auto tc = Render::activeCamera->GetComponent<TransformComponent>();
        tc->SetPosition(newPosition);
        tc->SetRotation(vec3(0, 0, 0));

        WriteConsoleMessage(
            Caller::INPUT,
            Type::INFO,
            "Reset camera position and rotation.\n");
    }

#endif
}