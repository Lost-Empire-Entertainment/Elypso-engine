//engine
#include "timestamp.h"
#include "console.h"
#include "logger.h"

//external
#include "magic_enum.hpp"

#include <string>
#include <iostream>

using namespace std;

namespace Core::Console
{
	Logger logger("engine_log.txt");

	void ConsoleManager::WriteConsoleMessage(Caller caller, Type type, const string& message)
	{
		string msg;

		switch (type)
		{
		default:
			msg =
				Timestamp::GetCurrentTimestamp() +
				string(magic_enum::enum_name(type)) +
				" is not a valid error type!";

			cerr << msg;
			logger.Log(msg);
			break;
		case ConsoleManager::Type::CLEANUP:
		case ConsoleManager::Type::DEBUG:
		case ConsoleManager::Type::INFO:
		case ConsoleManager::Type::SUCCESS:
			msg =
				Timestamp::GetCurrentTimestamp() +
				"[" +
				string(magic_enum::enum_name(caller)) +
				"_" +
				string(magic_enum::enum_name(type)) +
				"] " +
				message;

			cout << msg;
			logger.Log(msg);
			break;
		case ConsoleManager::Type::ERROR:
			msg =
				Timestamp::GetCurrentTimestamp() +
				"[" +
				string(magic_enum::enum_name(caller)) +
				"_" +
				string(magic_enum::enum_name(type)) +
				"] " +
				message;

			cerr << msg;
			logger.Log(msg);
			break;
		}
	}
}