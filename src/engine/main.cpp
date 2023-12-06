//engine
#include "core.h"

#include <iostream>

int main()
{
	if (Core::InitializeEngine() != 0) 
	{
		std::cin.get();
		return -1;
	}

	Core::RunEngine();

	Core::ShutdownEngine();
}