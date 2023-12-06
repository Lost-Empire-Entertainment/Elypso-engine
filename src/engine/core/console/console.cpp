//engine
#include "timestamp.h"
#include "console.h"
#include "logger.h"

//external
#include "magic_enum.hpp"

#include <string>
#include <iostream>

namespace Core::Console
{
	Logger logger("engine_log.txt");

	void ConsoleManager::WriteConsoleMessage(Caller caller, Type type, const std::string& message)
	{
		std::string msg;

		switch (type)
		{
		default:
			msg =
				Timestamp::GetCurrentTimestamp() +
				std::string(magic_enum::enum_name(type)) +
				" is not a valid error type!";

			std::cerr << msg;
			logger.Log(msg);
			break;
		case ConsoleManager::Type::CLEANUP:
		case ConsoleManager::Type::DEBUG:
		case ConsoleManager::Type::INFO:
		case ConsoleManager::Type::SUCCESS:
			msg =
				Timestamp::GetCurrentTimestamp() +
				"[" +
				std::string(magic_enum::enum_name(caller)) +
				"_" +
				std::string(magic_enum::enum_name(type)) +
				"] " +
				message;

			std::cout << msg;
			logger.Log(msg);
			break;
		case ConsoleManager::Type::ERROR:
			msg =
				Timestamp::GetCurrentTimestamp() +
				"[" +
				std::string(magic_enum::enum_name(caller)) +
				"_" +
				std::string(magic_enum::enum_name(type)) +
				"] " +
				message;

			std::cerr << msg;
			logger.Log(msg);
			break;
		}
	}
}