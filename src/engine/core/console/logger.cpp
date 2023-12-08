#include "logger.h"
#include "console.h"

using namespace std;

namespace Core::Console
{
	Logger::Logger(const string& logFileName)
	{
		logFile.open(logFileName, ios::out | ios::app);
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

	void Logger::Log(const string& message)
	{
		if (logFile.is_open())
		{
			logFile << message << endl;
		}
	}
}