#pragma once

#ifndef CORE_CONSOLE_H
#define CORE_CONSOLE_H

#include <iostream>
#include <string>

using namespace std;

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

		/// <summary>
		/// A simple method that prints a message with a message type and timestamp to the console.
		/// </summary>
		/// <param name="caller">What called this message?</param>
		/// <param name="type">What type of a message is this?</param>
		/// <param name="message">The actual message.</param>
		static void WriteConsoleMessage(Caller caller, Type type, const string& message);
	};
}

#endif