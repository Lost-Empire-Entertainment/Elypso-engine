#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <ctime>

class Logger
{
public:
	Logger(const std::string& logFileName);
	~Logger();

	void Log(const std::string& message);

private:
	std::ofstream logFile;
};