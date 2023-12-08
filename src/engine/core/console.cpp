//engine
#include "console.h"

//external
#include "magic_enum.hpp"

#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>

using namespace std;
using namespace std::chrono;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
    Logger logger("engine_log.txt");

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

    Logger::Logger(const string& logFileName)
    {
        logFile.open(logFileName, ios::out | ios::app);
        if (!logFile.is_open())
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE, 
                Type::ERROR, 
                "Error opening log file: " + logFileName + ".");
        }
    }

    Logger::~Logger()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void Logger::Log(const string& message)
    {
        if (logFile.is_open())
        {
            logFile << message << endl;
        }
    }

    void ConsoleManager::WriteConsoleMessage(Caller caller, Type type, const string& message)
    {
        string msg;

        switch (type)
        {
        default:
            msg = Timestamp::GetCurrentTimestamp() + string(magic_enum::enum_name(type)) + " is not a valid error type!";
            break;
        case Type::CLEANUP:
        case Type::DEBUG:
        case Type::INFO:
        case Type::SUCCESS:
            msg = Timestamp::GetCurrentTimestamp() + "[" + string(magic_enum::enum_name(caller)) + "_" + string(magic_enum::enum_name(type)) + "] " + message;
            break;
        case Type::ERROR:
            msg = Timestamp::GetCurrentTimestamp() + "[" + string(magic_enum::enum_name(caller)) + "_" + string(magic_enum::enum_name(type)) + "] " + message;
            break;
        }

        cerr << msg;
        logger.Log(msg);
    }
}