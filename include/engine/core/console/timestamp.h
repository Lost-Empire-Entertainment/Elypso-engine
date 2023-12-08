#pragma once

#ifndef CORE_CONSOLE_TIMESTAMP_H
#define CORE_CONSOLE_TIMESTAMP_H

#include <string>

using namespace std;

namespace Core::Console
{
	class Timestamp
	{
	public:
		static string GetCurrentTimestamp();
	};
}

#endif