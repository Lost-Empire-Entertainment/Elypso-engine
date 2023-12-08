//engine
#include "core.h"
#include "shutdown.h"

#include <iostream>

int main()
{
	Core::InitializeEngine();

	Core::RunEngine();

	Core::ShutdownManager::Shutdown();
	std::cin.get();
	return 0;
}