//engine
#include "timestamp.h"

//cpp
#include <iomanip>
#include <chrono>

//get current timestamp as string
std::string GetCurrentTimestamp()
{
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

	auto now_c = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now - now_ms);

	//extract hours, minutes, seconds
	auto tm = *std::localtime(&now_c);
	auto hours = tm.tm_hour;
	auto minutes = tm.tm_min;
	auto seconds = tm.tm_sec;

	//create the timestamp string
	std::stringstream ss;
	ss
		<< "["
		<< std::setw(2) << std::setfill('0') << hours << ":"
		<< std::setw(2) << std::setfill('0') << minutes << ":"
		<< std::setw(2) << std::setfill('0') << seconds << ":"
		<< std::setw(3) << ms.count() << "] ";

	return ss.str();
}