//engine
#include "timestamp.h"
#include "writeConsoleMessage.h"

//external
#include "magic_enum.hpp"

//cpp
#include <string>

//prints the message to the console
void WriteConsoleMessage(MessageType messageType, ErrorType errorType, const std::string& message)
{
	switch (errorType)
	{
	default:
		std::cerr
			<< GetCurrentTimestamp()
			<< magic_enum::enum_name(errorType)
			<< " is not a valid error type!";
		break;
	case INFO:
	case SUCCESS:
		std::cout
			<< GetCurrentTimestamp()
			<< "["
			<< magic_enum::enum_name(messageType)
			<< "_"
			<< magic_enum::enum_name(errorType)
			<< "] "
			<< message;
		break;
	case ERROR:
		std::cerr
			<< GetCurrentTimestamp()
			<< "["
			<< magic_enum::enum_name(messageType)
			<< "_"
			<< magic_enum::enum_name(errorType)
			<< "] "
			<< message;
		break;
	}
}