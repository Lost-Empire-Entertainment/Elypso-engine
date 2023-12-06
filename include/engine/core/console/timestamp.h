#pragma once

#ifndef CORE_CONSOLE_TIMESTAMP_H
#define CORE_CONSOLE_TIMESTAMP_H

#include <string>

namespace Core::Console
{
	class Timestamp
	{
	public:
		static std::string GetCurrentTimestamp();
	};
}

#endif