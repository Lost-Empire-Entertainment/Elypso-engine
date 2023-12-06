#pragma once

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