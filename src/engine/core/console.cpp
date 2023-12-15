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
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//external
#include "magic_enum.hpp"

//engine
#include "console.hpp"
#include "searchUtils.hpp"

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
using std::filesystem::remove;
using std::filesystem::exists;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::duration_cast;
using std::chrono::time_point_cast;
using std::chrono::system_clock;

using Utils::Search;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
    Logger logger(Search::FindDocumentsFolder() + "/" + "engine_log.txt");

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
        if (exists(logFileName))
        {
            remove(logFileName);
            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::CLEANUP,
                "Deleted file: engine_log.txt\n");
        }

        logFile.open(logFileName, ios::out | ios::app);
        if (!logFile.is_open())
        {
            error_code ec = make_error_code(static_cast<errc>(errno));

            ConsoleManager::WriteConsoleMessage(
                Caller::ENGINE,
                Type::EXCEPTION,
                "Failed to open log file " + logFileName + "! Reason: " +
                ec.message() + "\n\n");
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
        case Type::EXCEPTION:
            msg = Timestamp::GetCurrentTimestamp() + "[" + string(magic_enum::enum_name(caller)) + "_" + string(magic_enum::enum_name(type)) + "] " + message;
            break;
        }

        cout << msg;
        logger.Log(msg);
    }
}