#include "logger.h"
#include "console.h"

namespace Core::Console
{
	Logger::Logger(const std::string& logFileName)
	{
		logFile.open(logFileName, std::ios::out | std::ios::app);
		if (!logFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				ConsoleManager::Caller::ENGINE,
				ConsoleManager::Type::ERROR,
				"Error opening log file: " + logFileName + ".");
		}
	}

	Logger::~Logger()
	{
		if (logFile.is_open())
		{
			logFile.close();
		}
	}

	void Logger::Log(const std::string& message)
	{
		if (logFile.is_open())
		{
			logFile << message << std::endl;
		}
	}
}