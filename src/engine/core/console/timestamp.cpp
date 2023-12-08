//engine
#include "timestamp.h"

#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace Core::Console
{
	string Timestamp::GetCurrentTimestamp()
	{
		auto now = system_clock::now();
		auto now_ms = time_point_cast<milliseconds>(now);

		auto now_c = system_clock::to_time_t(now);
		auto ms = duration_cast<microseconds>(now - now_ms);

		auto tm = *localtime(&now_c);
		auto hours = tm.tm_hour;
		auto minutes = tm.tm_min;
		auto seconds = tm.tm_sec;

		stringstream ss;
		ss
			<< "["
			<< setw(2) << setfill('0') << hours << ":"
			<< setw(2) << setfill('0') << minutes << ":"
			<< setw(2) << setfill('0') << seconds << ":"
			<< setw(3) << ms.count() << "] ";

		return ss.str();
	}
}