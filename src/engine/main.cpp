//engine
#include "core.h"
#include "shutdown.h"

#include <iostream>

/*
* =========================================
* 
* The main script of the engine. 
* It should remain as is and is not recommended to be modified.
* core.cpp is responsible for calling the necessary 
* methods for initialization and running of the engine.
* 
* =========================================
*/

int main()
{
	Core::InitializeEngine();

	Core::RunEngine();

	Core::ShutdownManager::Shutdown();
	std::cin.get();
	return 0;
}