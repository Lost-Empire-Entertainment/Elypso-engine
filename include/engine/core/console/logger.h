#pragma once

#ifndef CORE_CONSOLE_LOGGER_H
#define CORE_CONSOLE_LOGGER_H

#include <fstream>
#include <iostream>
#include <string>
#include <ctime>

namespace Core::Console
{
	class Logger
	{
	public:
		Logger(const std::string& logFileName);
		~Logger();

		void Log(const std::string& message);

	private:
		std::ofstream logFile;
	};
}

#endif