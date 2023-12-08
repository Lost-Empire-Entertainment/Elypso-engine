#pragma once

#ifndef CORE_CONSOLE_LOGGER_H
#define CORE_CONSOLE_LOGGER_H

#include <fstream>
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

namespace Core::Console
{
	class Logger
	{
	public:
		Logger(const string& logFileName);
		~Logger();

		/// <summary>
		/// A simple method that prints the console message to an external txt file.
		/// </summary>
		/// <param name="message"></param>
		void Log(const string& message);

	private:
		ofstream logFile;
	};
}

#endif