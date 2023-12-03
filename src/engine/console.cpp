//engine
#include "timestamp.h"
#include "console.h"

//external
#include "magic_enum.hpp"

#include <string>
#include <iostream>

void ConsoleManager::WriteConsoleMessage(MessageType messageType, ErrorType errorType, const std::string& message)
{
	switch (errorType)
	{
	default:
		std::cerr
			<< Timestamp::GetCurrentTimestamp()
			<< magic_enum::enum_name(errorType)
			<< " is not a valid error type!";
		break;
	case ConsoleManager::ErrorType::INFO:
	case ConsoleManager::ErrorType::SUCCESS:
		std::cout
			<< Timestamp::GetCurrentTimestamp()
			<< "["
			<< magic_enum::enum_name(messageType)
			<< "_"
			<< magic_enum::enum_name(errorType)
			<< "] "
			<< message;
		break;
	case ConsoleManager::ErrorType::ERROR:
		std::cerr
			<< Timestamp::GetCurrentTimestamp()
			<< "["
			<< magic_enum::enum_name(messageType)
			<< "_"
			<< magic_enum::enum_name(errorType)
			<< "] "
			<< message;
		break;
	}
}