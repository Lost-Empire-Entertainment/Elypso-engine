//engine
#include "core.h"

int main()
{
	if (Core::InitializeEngine() != 0) 
	{
		return -1;
	}

	Core::RunEngine();

	Core::ShutdownEngine();
}