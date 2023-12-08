//engine
#include "core.h"
#include "shutdown.h"

#include <iostream>

using namespace std;
using namespace Core;

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
	InitializeEngine();

	RunEngine();

	ShutdownManager::Shutdown();
	cin.get();
	return 0;
}