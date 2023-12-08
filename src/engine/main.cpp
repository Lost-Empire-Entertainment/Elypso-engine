//engine
#include "core.h"
#include "shutdown.h"

#include <iostream>

using namespace std;
using namespace Core;

int main()
{
	InitializeEngine();

	RunEngine();

	ShutdownManager::Shutdown();
	cin.get();
	return 0;
}