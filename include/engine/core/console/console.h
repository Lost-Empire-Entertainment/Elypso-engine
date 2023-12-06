#pragma once

#ifndef CORE_CONSOLE_H
#define CORE_CONSOLE_H

#include <iostream>
#include <string>

namespace Core::Console
{
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
			TEXTURE,
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
}

#endif