#pragma once

#include <iostream>
#include <string>

class ConsoleManager
{
public:
	enum class MessageType : char
	{
		ENGINE,
		INPUT,
		SHADER,
		WINDOW_SETUP,
		WINDOW_LOOP,
		GLFW,
		GLAD,
		IMGUI,
		SHUTDOWN
	};
	enum class ErrorType : char
	{
		SUCCESS,
		INFO,
		ERROR
	};

	static void WriteConsoleMessage(MessageType messageType, ErrorType errorType, const std::string& message);
};