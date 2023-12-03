#pragma once

#include <iostream>
#include <string>

class ConsoleManager
{
public:
	enum class Caller : char
	{
		ENGINE,
		INPUT,
		SHADER,
		WINDOW_SETUP,
		WINDOW_LOOP,
		OPENGL,
		GLFW,
		GLAD,
		IMGUI,
		SHUTDOWN
	};
	enum class Type : char
	{
		SUCCESS,
		INFO,
		DEBUG,
		CLEANUP,
		ERROR
	};

	static void WriteConsoleMessage(Caller caller, Type type, const std::string& message);
};