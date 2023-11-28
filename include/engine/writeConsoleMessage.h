#pragma once

#include <iostream>
#include <string>

enum MessageType {
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
enum ErrorType {
	SUCCESS,
	INFO,
	ERROR
};

void WriteConsoleMessage(MessageType messageType, ErrorType errorType, const std::string& message);