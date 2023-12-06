#pragma once

#include <string>

namespace Core
{
	extern const std::string name;
	extern const std::string version;

	int InitializeEngine();
	void RunEngine();
	int ShutdownEngine();
}